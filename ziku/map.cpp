#include <map>
#include <string>
#include <utility>
using namespace std;

int main()
{
	//定义作者名数组
	string authors[] = {"Stanley B.Lippman", "Scott Meyers"};
	//定义作者名-书名multimap
	multimap<string, string> auth_books;

	//向auth_books中插入信息
	auth_books.insert(make_pair(authors[0], string("c++ primer")));
	auth_books.insert(make_pair(authors[0], string("essential c++")));
	auth_books.insert(make_pair(authors[1], string("effective c++")));
	auth_books.insert(make_pair(authors[1], string("more effective c++")));
	auth_books.insert(make_pair(authors[1], string("effective stl")));
	
	/****************************************************/
	//第一种方式，使用find和count查找作者对应的书名信息并输出
	//cout << "========================" << endl;
	typedef multimap<string, string>::size_type sz_type;
	sz_type num = auth_books.count(authors[0]);
	multimap<string, string>::iterator it = auth_books.find(authors[0]);

	//因为在multimap和multiset中具有相同键的元素相邻存放，所以可通过迭代器依次访问
	//输出查找结果
	for (sz_type cnt = 0; cnt != num; ++cnt, ++it)
		cout << it->second << endl;
	//cout << "========================" << endl;

	/***************************************************/
	//第二种方式，lower_bound和upper_bound
	typedef multimap<string, string>::iterator auth_it;
	auth_it beg = auth_books.lower_bound(authors[1]);
	auth_it end = auth_books.upper_bound(authors[1]);
	while (beg != end){
		cout << beg->second << endl;
		++beg;
	}
	//cout << "========================" << endl;

	/***************************************************/
	//第三种方式，equal_range
	pair<auth_it, auth_it> pos = auth_books.equal_range(authors[0]);
	while (pos.first != pos.second){//pos.first等价于lower_bound()，pos.second等价于upper_bound()
		cout << pos.first->second << endl;
		++pos.first;
	}
	//cout << "========================" << endl;

	return 0;
}