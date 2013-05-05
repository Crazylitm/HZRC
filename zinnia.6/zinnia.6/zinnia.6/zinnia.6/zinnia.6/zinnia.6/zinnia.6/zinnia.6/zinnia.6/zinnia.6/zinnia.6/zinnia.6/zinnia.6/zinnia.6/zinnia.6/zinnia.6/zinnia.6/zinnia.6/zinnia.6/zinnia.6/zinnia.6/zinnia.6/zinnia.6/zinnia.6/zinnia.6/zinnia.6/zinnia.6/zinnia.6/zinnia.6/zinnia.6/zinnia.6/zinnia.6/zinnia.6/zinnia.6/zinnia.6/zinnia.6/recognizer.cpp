//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: recognizer.cpp 27 2010-05-09 05:34:05Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include "common.h"
#include "mmap.h"
#include "zinnia.h"
#include "feature.h"

namespace {

static inline char *read_ptr(char **ptr, size_t size) {
  char *r = *ptr;
  *ptr += size;
  return r;
}

template <typename T>
inline void read_static(char **ptr, T *value) {
  char *r = read_ptr(ptr, sizeof(T));
  memcpy(value, r, sizeof(T));
}

#ifndef WORDS_LITENDIAN
template <>
inline void read_static<unsigned int>(char **ptr, unsigned int *value) {
    unsigned char *buf = reinterpret_cast<unsigned char *>(*ptr);
    *value = (buf[0]) | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    *ptr += 4;
}

template <>
inline void read_static<float>(char **ptr, float *value) {
    unsigned int x;
    read_static<unsigned int>(ptr, &x);
    memcpy(value, &x, sizeof(x));
}
#endif

}

namespace zinnia {
    
struct ResultNote
    {
        int iTimeNum;
        int iStorkeNum;
        float x;
        float y;
    };
class ResultImpl: public Result {
 public:
  void add(const char *character, float score) {
    results_.push_back(std::make_pair(score, character));
  }
  void add(ResultNote x,float y) {
        results_notepairs_.push_back(std::make_pair(x, y));
  }
  void clear() { results_.clear(); results_notepairs_.clear(); }
  const char *value(size_t i) const {
    return (i >= results_.size()) ? 0 : results_[i].second;
  }
  float score(size_t i) const {
    return (i >= results_.size()) ? -1 : results_[i].first;
  }
    float getx(size_t i) const{
        return (i >= results_notepairs_.size()) ? -1 : results_notepairs_[i].first.x;
    }
    float gety(size_t i) const{
        return (i >= results_notepairs_.size()) ? -1 : results_notepairs_[i].first.y;
    }
    int getTimeNum(size_t i) const{
        return (i >= results_notepairs_.size()) ? -1 : results_notepairs_[i].first.iTimeNum;
    }
    int getStorkeNum(size_t i) const{
        return (i >= results_notepairs_.size()) ? -1 : results_notepairs_[i].first.iStorkeNum;
    }
  size_t size() const { return results_.size(); }
  size_t size_pairs() const {return results_notepairs_.size();}
  //std::vector<int > iTimeNum_;
  ResultImpl() {}
  virtual ~ResultImpl() {}
 private:
  std::vector<std::pair<float, const char*> > results_;
  std::vector<std::pair<ResultNote, float> > results_notepairs_;
    
};

class RecognizerImpl: public Recognizer {
 public:
  bool open(const char *filename);
  bool open(const char *ptr, size_t size);
  bool close();
  size_t size() const { return model_.size(); }
  const char *value(size_t i) const;
  Result* classify(const Character &character,
                   size_t nbest) const;
  Result* classify(const Character &character) const;
  const char *what() { return what_.str(); }
  explicit RecognizerImpl() {}
  virtual ~RecognizerImpl() { close(); }

 private:
  struct Model {
    const char *character;
    float bias;
    const FeatureNode *x;
  };

  Mmap<char> mmap_;
  std::vector<Model> model_;
  whatlog what_;
};

const char *RecognizerImpl::value(size_t i) const {
  return (i >= model_.size()) ? 0 : model_[i].character;
}

bool RecognizerImpl::open(const char *filename) {
  CHECK_FALSE(mmap_.open(filename))
      << "no such file or directory: " << filename;
  model_.clear();
  return open(mmap_.begin(), mmap_.file_size());
}

bool RecognizerImpl::open(const char *p, size_t ptr_size) {
  char *ptr = const_cast<char *>(p);
  const char *begin = ptr;
  const char *end = ptr + ptr_size;
  unsigned int version = 0;
  unsigned int magic = 0;
  read_static<unsigned int>(&ptr, &magic);
  CHECK_CLOSE_FALSE((magic ^ DIC_MAGIC_ID) == ptr_size)
      << "model file is broken";

  read_static<unsigned int>(&ptr, &version);
  CHECK_CLOSE_FALSE(version == DIC_VERSION)
      << "incompatible version: " << version;

  unsigned int size = 0;
  read_static<unsigned int>(&ptr, &size);

  model_.resize(size);
  for (size_t i = 0; i < size; ++i) {
    Model &m = model_[i];
    m.character = read_ptr(&ptr, 16);
    CHECK_CLOSE_FALSE(ptr < end) << "model file is broken";
    float bias = 0.0;
    read_static<float>(&ptr, &bias);
    m.bias = bias;
    m.x = const_cast<const FeatureNode *>
        (reinterpret_cast<FeatureNode *>(ptr));
    size_t len = 0;
    for (const FeatureNode *x = m.x; x->index != -1; ++x) ++len;
    CHECK_CLOSE_FALSE(ptr < end) << "model file is broken";
    ptr += sizeof(FeatureNode) * (len + 1);
  }

  CHECK_FALSE(static_cast<size_t>(ptr - begin) == ptr_size)
      << "size of model file is invalid";

  return true;
}

bool RecognizerImpl::close() {
  mmap_.close();
  model_.clear();
  return true;
}
    
    Result *RecognizerImpl::classify(const Character &character,
                                     size_t nbest) const {
        if (model_.empty() || nbest <= 0) {
            return 0;
        }
        
        Features feature;
        if (!feature.read(character)) {
            return 0;
        }
        const FeatureNode *x = feature.get();
        
        std::vector<std::pair<float, const char*> > results(size());
        for (size_t i = 0; i < model_.size(); ++i) {
            results[i].first  = model_[i].bias + dot(model_[i].x, x);
            results[i].second = model_[i].character;
        }
        
        nbest = _min(nbest, results.size());
        
        std::partial_sort(results.begin(),
                          results.begin() + nbest, results.end(),
                          std::greater<std::pair<float, const char*> >());
        
        ResultImpl *result = new ResultImpl;
        for (size_t i = 0; i < nbest; ++i)
            result->add(results[i].second, results[i].first);
        
        return result;
    }

Result *RecognizerImpl::classify(const Character &character) const {

  Features feature;
  if (!feature.read(character)) {
    return 0;
  }
  const FeatureNodePair *x = feature.getPair();
#define PT_SIZE 100000
    struct Resultnote {
        float x;
        float y;
        int iTimenum;
    } *presultnote,PT[PT_SIZE];
  
    int findDubblefirst=0 ,findDoublelast=0;
    /*
    if(feature.size()>0)
    {
        presultnote  = new Resultnote [feature.size()];
    }*/
    presultnote = PT;
    int iNum=0;
    int itemp1 = feature.size() ;
    ResultImpl *result = new ResultImpl;

    presultnote[0].iTimenum = -1;
    for (size_t i = 0; i < feature.size() && i<PT_SIZE ; ++i){
        findDubblefirst=0;findDoublelast=0;
            //所有presultnote中的值与x.first进行比较，看是否重复
            for(size_t j =0;j < iNum;++j){
                int ix=0,iy=0;
                ix = presultnote[j].iTimenum;
                iy = x[i].first.iTimeNum;
                if (presultnote[j].iTimenum == x[i].first.iTimeNum)
                {
                  findDubblefirst=1;
                    break;
                }
                
                
            }
            //所有presultnote中的值与x.last进行比较，看是否重复
            for(size_t j =0;j < iNum;++j){
                int ix=0,iy=0;
                ix = presultnote[j].iTimenum;
                iy = x[i].last.iTimeNum;
                if (presultnote[j].iTimenum == x[i].last.iTimeNum)
                {
                    findDoublelast=1;
                    break;
                }
                
            }
        
        if(findDubblefirst == 0)
        {
            presultnote[iNum].x = x[i].first.x;
            presultnote[iNum].y = x[i].first.y;   
            presultnote[iNum].iTimenum = x[i].first.iTimeNum;
            iNum++;
        }
        if(findDoublelast == 0)
        {
            presultnote[iNum].x = x[i].last.x;
            presultnote[iNum].y = x[i].last.y;   
            presultnote[iNum].iTimenum = x[i].last.iTimeNum;
            iNum++;
        }
        
    }

    
    
    struct Resultnote iMinNote, iSwichNote;
    for(size_t i = 0; i < iNum ; ++i)
    {
            iMinNote.x = presultnote[i].x;
            iMinNote.y = presultnote[i].y;
            itemp1 = presultnote[i].iTimenum;
            iMinNote.iTimenum = presultnote[i].iTimenum;


        for(size_t j =i+1 ;j< iNum; ++j)
        {
            if(iMinNote.iTimenum < presultnote[j].iTimenum) continue;
            iSwichNote.x =  iMinNote.x;
            iSwichNote.y =  iMinNote.y;
            iSwichNote.iTimenum =  iMinNote.iTimenum;
            
            iMinNote.x = presultnote[j].x;
            iMinNote.y = presultnote[j].y;
            itemp1 = presultnote[j].iTimenum;
            iMinNote.iTimenum = presultnote[j].iTimenum;
            
            presultnote[j].x =  iSwichNote.x;
            presultnote[j].y =  iSwichNote.y;
            presultnote[j].iTimenum =  iSwichNote.iTimenum;
        }
        ResultNote f;
        f.x = iMinNote.x;
        f.y = iMinNote.y;
        f.iTimeNum = iMinNote.iTimenum ;
        f.iStorkeNum = (iMinNote.iTimenum)/10000;
        result->add(f,0.0);

    }
/*
    ResultImpl *result = new ResultImpl;
        for (size_t i = 0; i < feature.size() ; ++i)
        {
            ResultNote f;
            f.x = x[i].first.x;
            f.y = x[i].first.y;
            f.iTimeNum = x[i].first.iTimeNum ;
           // f.iStorkeNum = (iMinNote.iTimenum)/10000;
            result->add(f,0.0);    
      
            f.x = x[i].last.x;
            f.y = x[i].last.y;
            f.iTimeNum = x[i].last.iTimeNum ;
            // f.iStorkeNum = (iMinNote.iTimenum)/10000;
            result->add(f,0.0);   
        
        }
 */
  //  delete presultnote;
  return result;
}

Recognizer* Recognizer::create() {
  return new RecognizerImpl;
}

Recognizer* createRecognizer() {
  return new RecognizerImpl;
}
}
