#include "myLRUCache.h"

//unordered_map+一个双向链表：实现LRU读存

//Node为基本节点，保存(key,value),
//Node构成一个双向链表
class Node {
public:
	int key;
	int value;
	Node* pre = nullptr;
	Node* post = nullptr;
	Node() :key(0), value(0) {}
	Node(int k, int v) :key(k), value(v) {}
};
//myLRUCache LRU最近最久未使用缓存算法类，
//表示最先淘汰最近最久未使用的数据
class myLRUCache {
	//capacity缓存容量
	//size已缓存大小
	//map记录节点与key值，方便在O(1)时间内确定key是否存在
	//head为节点头指针，tail为节点尾指针
	//访问时间由最近到最早为从头指针到尾指针
	unordered_map<int, Node*> map;
	Node* head;
	Node* tail;
	int size;
	int capacity;
public:
	myLRUCache(int _capacity) {
		head = new Node();
		tail = new Node();
		head->post = tail;
		tail->pre = head;
		capacity = _capacity;
		size = 0;
	}
	~myLRUCache() {
		delete head;
		delete tail;
		for (unordered_map<int, Node*>::iterator iter = map.begin(); iter != map.end(); iter++) {
			delete iter->second;
		}
	}
	//将节点放入头结点后，表示最近访问的节点
	void putNode(Node* node) {
		node->post = head->post;
		head->post->pre = node;
		head->post = node;
		node->pre = head;
	}
	//删除节点，返回节点key值
	int removeNode(Node* node) {
		node->pre->post = node->post;
		node->post->pre = node->pre;
		return node->key;
	}
	//将原节点删除后放入头结点后，表示最近访问的节点
	void moveToHead(Node* node)
	{
		removeNode(node);
		putNode(node);
	}
	//得到key键值的值，若存在则该键值的节点前移
	int get(int key) {
		if (capacity == 0) return -1;
		auto it = map.find(key);
		if (it == map.end()) return -1;
		moveToHead(it->second);
		return it->second->value;
	}
	//放入（key,value）节点
	void put(int key, int value) {
		if (capacity == 0) return;
		auto it = map.find(key);
		if (it == map.end()) {
			if (capacity == size) {
				int temkey = removeNode(tail->pre);
				map.erase(temkey);
				size--;
			}
			Node* newNode = new Node(key, value);
			putNode(newNode);
			map.insert(make_pair(key, newNode));
			size++;
		}
		else {
			it->second->value = value;
			moveToHead(it->second);
		}
	}
};