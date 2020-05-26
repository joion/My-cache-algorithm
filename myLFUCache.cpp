#include "myLFUCache.h"

//unordered_map+两个双向链表：实现LFU读存

class freNode;

//Node为基本节点，保存(key,value),并保存freNode指针，指向该节点所在频率节点
//Node构成一个双向链表
class Node {
public:
	int key;
	int value;
	Node* pre = NULL;
	Node* next = NULL;
	freNode* freqNode = NULL;
	Node() {
		key = 0;
		value = 0;
	}
	Node(int k, int v) :key(k), value(v) {}

};
//freNode为频率节点，保存频率fre，内部含有一条Node组成的双向链表的头尾指针
//headNode为该频率下的头指针，tailNode为该频率下尾指针
//节点创建从新到旧为从头指针到尾指针
//pre指向上一个频率节点，next指向下一个频率节点
//freNode构成一个双向链表
class freNode {
public:
	int fre;
	freNode* pre = NULL;
	freNode* next = NULL;
	Node* headNode = NULL;
	Node* tailNode = NULL;
	freNode() {
		fre = 0;
		headNode = new Node();
		tailNode = new Node();
		headNode->next = tailNode;
		tailNode->pre = headNode;
	}
	freNode(int f) {
		fre = f;
		headNode = new Node();
		tailNode = new Node();
		headNode->next = tailNode;
		tailNode->pre = headNode;
	}
	~freNode() {
		fre = 0;
		delete headNode;
		delete tailNode;
	}
	//判断该频率节点中是否有Node节点存在，为方便之后删除
	bool isNull() {
		if (headNode->next == tailNode) {
			return true;
		}
		return false;
	}

};

//myLFUCache LFU最近最少使用缓存算法类，
//表示最先淘汰最近使用频次最少的数据，即在使用频率最少的节点中选择最早访问的节点淘汰
class myLFUCache {
	//capacity缓存容量
	//size已缓存大小
	//map记录节点与key值，方便在O(1)时间内确定key是否存在
	//head为频率节点头指针，tail为频率节点尾指针
	//频率由高到低为从头指针到尾指针
	int capacity;
	int size;
	unordered_map<int, Node*> map;
	freNode* head = NULL;
	freNode* tail = NULL;

public:
	myLFUCache(int _capacity) {
		capacity = _capacity;
		size = 0;
		head = new freNode();
		tail = new freNode();
		head->next = tail;
		tail->pre = head;
	}
	~myLFUCache() {
		capacity = 0;
		size = 0;
		delete head;
		delete tail;
		for (unordered_map<int, Node*>::iterator iter = map.begin(); iter != map.end(); iter++) {
			delete iter->second;
		}
	}
	//删除一个节点，返回删除节点
	Node* deleteNode(Node* node) {
		node->next->pre = node->pre;
		node->pre->next = node->next;
		return node;
	}
	//删除一个频率节点，返回该频率节点的下一个频率节点
	freNode* deleteFreNode(freNode* node) {
		freNode* nextFreNode = node->next;
		node->next->pre = node->pre;
		node->pre->next = node->next;
		return nextFreNode;
	}
	//在nextFreNode前面新建一个fre=freq的频率节点，返回该频率节点
	freNode* newFreNodeInList(int freq, freNode* nextFreNode) {
		freNode* node = new freNode(freq);
		freNode* tem = nextFreNode->pre;
		nextFreNode->pre = node;
		tem->next = node;
		node->pre = tem;
		node->next = nextFreNode;
		return node;
	}
	//在head头结点后面新建一个fre=freq的频率节点，返回该频率节点
	freNode* newFreNodeHead(int freq) {
		freNode* node = new freNode(freq);
		freNode* tem = head->next;
		head->next = node;
		tem->pre = node;
		node->pre = head;
		node->next = tem;
		return node;
	}
	//将node节点插入freqNode频率节点中链表的头结点headNode后面
	void insertFreNode(Node* node, freNode* freqNode) {
		Node* tem = freqNode->headNode->next;
		freqNode->headNode->next = node;
		tem->pre = node;
		node->pre = freqNode->headNode;
		node->next = tem;
	}
	//将node节点放入freq+1的频率节点中
	void addFre(Node*& node) {
		//先判断该节点所在频率节点是否为空
		//若为空了，则删除该频率节点
		//查看该频率节点下一个节点是否为freq + 1
		//将节点加入freq+1的频率节点中
		if (node->freqNode->isNull()) {
			int freq = node->freqNode->fre;
			freNode* dfreNode = deleteFreNode(node->freqNode);
			if (dfreNode->fre == freq + 1) {
				insertFreNode(node, dfreNode);
				node->freqNode = dfreNode;
			}
			else {
				freNode* newFreqNode = newFreNodeInList(freq + 1, dfreNode);
				insertFreNode(node, newFreqNode);
				node->freqNode = newFreqNode;
			}
		}
		//若不为空
		//查看该频率节点下一个节点是否为freq + 1
		//将节点加入freq+1的频率节点中
		else {
			int freq = node->freqNode->fre;
			freNode* nfreNode = node->freqNode->next;
			if (nfreNode->fre == freq + 1) {
				insertFreNode(node, nfreNode);
				node->freqNode = nfreNode;
			}
			else {
				freNode* newFreqNode = newFreNodeInList(freq + 1, nfreNode);
				insertFreNode(node, newFreqNode);
				node->freqNode = newFreqNode;
			}
		}
	}
	//将新node节点放入fre=1的频率节点中
	void addNewFre(Node*& node) {
		if (head->next->fre != 1) {
			freNode* newFreqNode = newFreNodeHead(1);
			insertFreNode(node, newFreqNode);
			node->freqNode = newFreqNode;
		}
		else {
			insertFreNode(node, head->next);
			node->freqNode = head->next;
		}
	}
	//得到key键值的值，若存在则该键值的访问频次加1
	int get(int key) {
		if (capacity == 0) return -1;
		if (map.find(key) == map.end()) {
			return -1;
		}
		else {
			Node* node = deleteNode(map[key]);
			int res = node->value;
			addFre(node);
			map[key] = node;
			return res;
		}
	}
	//放入（key,value）节点
	void put(int key, int value) {
		if (capacity == 0) return;
		if (size < capacity) {
			if (map.find(key) == map.end()) {
				Node* node = new Node(key, value);
				addNewFre(node);
				map[key] = node;
				size++;
			}
			else {
				//注意不需要size++;
				Node* node = deleteNode(map[key]);
				node->value = value;
				addFre(node);
				map[key] = node;
			}
		}
		else if (size == capacity) {
			if (map.find(key) == map.end()) {
				Node* dNode = deleteNode(head->next->tailNode->pre);
				if (dNode->freqNode->isNull()) {
					deleteFreNode(dNode->freqNode);
				}
				//记得要删除map中淘汰的关键字
				map.erase(dNode->key);
				Node* node = new Node(key, value);
				addNewFre(node);
				map[key] = node;
			}
			else {
				Node* node = deleteNode(map[key]);
				node->value = value;
				addFre(node);
				map[key] = node;
			}
		}
	}
};


void test() {
	myLFUCache cache = myLFUCache(2 /* capacity (缓存容量) */);

	cache.put(10, 13);//null
	cache.put(3, 17);//null
	cache.put(6, 11);//
	cache.put(10, 5);//
	cache.put(9, 10);//
	cache.get(13);//-1
	cache.put(2, 19);//
	cache.get(2);//19
	cache.get(3);//-1
	cache.put(5, 25);//
	cache.get(8);//-1
	cache.put(9, 22);//
	cache.put(5, 5);//
	cache.put(1, 30);//
	cache.get(11);//-1       
	cache.put(9, 12);//
	cache.get(7);//-1
	cache.get(5);//-1
	cache.get(8);//-1
	cache.get(9);//12
	cache.put(4, 30);//
	cache.put(9, 3);//
	cache.get(9);//3
	cache.get(10);//-1
	cache.get(10);//-1
	cache.put(6, 14);//
	cache.put(3, 1);//
	cache.get(3);//1
	cache.put(10, 11);//
	cache.get(8);//-1
	cache.put(2, 14);//
	cache.get(1);//-1
	cache.get(5);//-1
	cache.get(4);//-1
	cache.put(11, 4);//  
	cache.put(12, 24);//   
	cache.put(5, 18);//
	cache.get(13);//-1
	cache.put(7, 23);//
	cache.get(8);//-1       
	cache.get(12);//-1
	cache.put(3, 27);//
	cache.put(2, 12);//
	cache.get(5);//-1     
	cache.put(2, 9);//
	cache.put(13, 4);//
	cache.put(8, 18);//
	cache.put(1, 7);//
	cache.get(6);//-1
	cache.put(9, 29);//
	cache.put(8, 21);//
	cache.get(5);//-1      
	cache.put(6, 30);//
	cache.put(1, 12);//
	cache.get(10);//-1
	cache.put(4, 15);//
	cache.put(7, 22);//
	cache.put(11, 26);//
	cache.put(8, 17);
	cache.put(9, 29);
	cache.get(5);//-1
	cache.put(3, 4);
	cache.put(11, 30);
	cache.get(12);//-1
	cache.put(4, 29);
	cache.get(3);//-1
	cache.get(9);//29
	cache.get(6);//-1
	cache.put(3, 4);
	cache.get(1);//-1
	cache.get(10);//-1
	cache.put(3, 29);
	cache.put(10, 28);
	cache.put(1, 20);
	cache.put(11, 13);
	cache.get(3);//-1
	cache.put(3, 12);
	cache.put(3, 8);
	cache.put(10, 9);
	cache.put(3, 26);
	return;
}