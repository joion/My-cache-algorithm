#include "myLRUCache.h"

//unordered_map+һ��˫������ʵ��LRU����

//NodeΪ�����ڵ㣬����(key,value),
//Node����һ��˫������
class Node {
public:
	int key;
	int value;
	Node* pre = nullptr;
	Node* post = nullptr;
	Node() :key(0), value(0) {}
	Node(int k, int v) :key(k), value(v) {}
};
//myLRUCache LRU������δʹ�û����㷨�࣬
//��ʾ������̭������δʹ�õ�����
class myLRUCache {
	//capacity��������
	//size�ѻ����С
	//map��¼�ڵ���keyֵ��������O(1)ʱ����ȷ��key�Ƿ����
	//headΪ�ڵ�ͷָ�룬tailΪ�ڵ�βָ��
	//����ʱ�������������Ϊ��ͷָ�뵽βָ��
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
	//���ڵ����ͷ���󣬱�ʾ������ʵĽڵ�
	void putNode(Node* node) {
		node->post = head->post;
		head->post->pre = node;
		head->post = node;
		node->pre = head;
	}
	//ɾ���ڵ㣬���ؽڵ�keyֵ
	int removeNode(Node* node) {
		node->pre->post = node->post;
		node->post->pre = node->pre;
		return node->key;
	}
	//��ԭ�ڵ�ɾ�������ͷ���󣬱�ʾ������ʵĽڵ�
	void moveToHead(Node* node)
	{
		removeNode(node);
		putNode(node);
	}
	//�õ�key��ֵ��ֵ����������ü�ֵ�Ľڵ�ǰ��
	int get(int key) {
		if (capacity == 0) return -1;
		auto it = map.find(key);
		if (it == map.end()) return -1;
		moveToHead(it->second);
		return it->second->value;
	}
	//���루key,value���ڵ�
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