//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: zinnia.cpp 9 2008-09-20 12:11:02Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#include "zinnia.h"
/*
int main(int argc, char** argv) {
  return zinnia_do(argc, argv);
}
 */
static const char *input =
"(character (width 1000)(height 1000)"
"(strokes ((92 124)(92 125)(92 130)(92 133)(92 138)(92 141)(92 143)(92 144)(92 148)(92 151)(92 155)(93 160)(93 163)(93 165)(93 166)(94 167))((95 130)(97 130)(100 130)(106 130)(112 130)(118 130)(125 130)(132 130)(137 130)(143 130)(148 130)(152 130)(155 130)(159 130)(161 130)(163 130)(166 130)(170 128)(185 126)(188 125)(191 125)(191 124)(191 126)(191 131)(191 134)(191 138)(191 141)(191 144)(191 149)(191 155)(191 159)(191 163)(191 165)(191 167)(191 168)(191 169)(190 169))((94 166)(97 166)(103 166)(109 166)(116 166)(123 166)(129 166)(135 166)(140 166)(148 166)(151 166)(155 166)(159 166)(164 166)(167 166)(170 166)(171 166)(172 166)(174 166)(176 166)(177 166)(179 166)(181 166)(183 166)(186 166)(188 166)(189 166)(190 166))((73 220)(79 234)(81 239)(82 243)(85 247)(86 250)(87 253)(87 257)(89 259)(89 261)(89 262)(89 267)(90 269)(91 271)(91 273))((75 217)(78 217)(88 217)(102 217)(116 217)(165 216)(175 214)(182 214)(188 212)(193 211)(197 209)(200 209)(204 209)(207 209)(213 209)(218 209)(225 209)(230 209)(232 209)(234 209)(234 210)(235 210)(235 211)(236 214)(237 217)(239 224)(239 226)(239 231)(239 236)(238 240)(236 243)(233 248)(231 252)(228 255)(226 259)(225 260)(224 262)(223 263)(222 263))((98 270)(104 270)(111 270)(120 270)(131 270)(141 270)(155 270)(162 270)(164 270)(168 270)(173 270)(178 270)(184 270)(187 270)(191 270)(194 270)(196 270)(197 270)(198 268)(200 268)(202 268)(206 267)(211 267)(215 266)(218 265)(220 265)(221 265)(222 265)(223 265)(224 265))((145 109)(145 110)(145 195)(145 215)(145 248)(145 264)(145 273)(145 284)(145 292)(145 295)(146 304)(146 309)(147 317)(147 323)(147 328)(147 334)(147 340)(147 347)(147 351)(147 354)(148 357)(148 359))";

/*
"(strokes ((200 200)(400 200))  \
          ((300 100)(300 500))  \
          ((300 200)(150 500))  \
          ((300 200)(450 500))))";
*/
//"(strokes ((243 273)(393 450))((700 253)(343 486)(280 716)(393 866)(710 880))))";

int recog(char *myOutString) {
    zinnia_recognizer_t *recognizer = zinnia_recognizer_new();
    zinnia_character_t  *character  = zinnia_character_new();
    if (!zinnia_recognizer_open(recognizer, "/usr/local/lib/zinnia/model/tomoe/handwriting-zh_CN.model")) {
        fprintf(stderr, "ERROR1: %s\n", zinnia_recognizer_strerror(recognizer));
        return -1;
    }
    
    if (!zinnia_character_parse(character, input)) {
        fprintf(stderr, "ERROR: %s %s\n", zinnia_character_strerror(character), input);
        return -1;
    }
    
    {
        zinnia_result_t *result = zinnia_recognizer_classify(recognizer, character, 10);
        size_t i; 
        if (!result) {
            fprintf(stderr, "ERROR: %s %s\n", zinnia_recognizer_strerror(recognizer), input);
            return -1;
        }
        for (i = 0; i < zinnia_result_size(result); ++i) {
            if(strlen(myOutString)<=0 || i==0)
            sprintf(myOutString, "%s\t%f\n",
                    zinnia_result_value(result, i),
                    zinnia_result_score(result, i));
            else
            {
                char stemp[2000];
                sprintf(stemp, "%s\t%f\n",
                        zinnia_result_value(result, i),
                        zinnia_result_score(result, i));
                strcat(myOutString, stemp);
            }
                
        }
        zinnia_result_destroy(result);
    }
    /*
    zinnia_character_clear(character);
    zinnia_character_set_width(character, 300);
    zinnia_character_set_height(character, 300);
    zinnia_character_add(character, 0, 51, 29);
    zinnia_character_add(character, 0, 117, 41);
    zinnia_character_add(character, 1, 99, 65);
    zinnia_character_add(character, 1, 219, 77);
    zinnia_character_add(character, 2, 27, 131);
    zinnia_character_add(character, 2, 261, 131);
    zinnia_character_add(character, 3, 129, 17);
    zinnia_character_add(character, 3, 57, 203);
    zinnia_character_add(character, 4, 111, 71);
    zinnia_character_add(character, 4, 219, 173);
    zinnia_character_add(character, 5, 81, 161);
    zinnia_character_add(character, 5, 93, 281);
    zinnia_character_add(character, 6, 99, 167);
    zinnia_character_add(character, 6, 207, 167);
    zinnia_character_add(character, 6, 189, 245);
    zinnia_character_add(character, 7, 99, 227);
    zinnia_character_add(character, 7, 189, 227);
    zinnia_character_add(character, 8, 111, 257);
    zinnia_character_add(character, 8, 189, 245);
    
    {
        zinnia_result_t *result = zinnia_recognizer_classify(recognizer, character, 10);
        size_t i;  
        if (!result) {
            fprintf(stderr, "%s\n", zinnia_recognizer_strerror(recognizer));
            return -1;
        }
        /// *
        for (i = 0; i < zinnia_result_size(result); ++i) {
            sprintf(myOutString, "%s\t%f\n",
                    zinnia_result_value(result, i),
                    zinnia_result_score(result, i));
        }* /
        for (i = 0; i < zinnia_result_size(result); ++i) {
            if(strlen(myOutString)<=0)
                sprintf(myOutString, "%s\t%f\n",
                        zinnia_result_value(result, i),
                        zinnia_result_score(result, i));
            else
            {
                char stemp[2000];
                sprintf(stemp, "%s\t%f\n",
                        zinnia_result_value(result, i),
                        zinnia_result_score(result, i));
                strcat(myOutString, stemp);
            }
            
        }
        zinnia_result_destroy(result);
    }
    */
    zinnia_character_destroy(character);
    zinnia_recognizer_destroy(recognizer);
    
    return 0;
}
