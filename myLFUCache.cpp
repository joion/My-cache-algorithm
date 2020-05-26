#include "myLFUCache.h"

//unordered_map+����˫������ʵ��LFU����

class freNode;

//NodeΪ�����ڵ㣬����(key,value),������freNodeָ�룬ָ��ýڵ�����Ƶ�ʽڵ�
//Node����һ��˫������
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
//freNodeΪƵ�ʽڵ㣬����Ƶ��fre���ڲ�����һ��Node��ɵ�˫�������ͷβָ��
//headNodeΪ��Ƶ���µ�ͷָ�룬tailNodeΪ��Ƶ����βָ��
//�ڵ㴴�����µ���Ϊ��ͷָ�뵽βָ��
//preָ����һ��Ƶ�ʽڵ㣬nextָ����һ��Ƶ�ʽڵ�
//freNode����һ��˫������
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
	//�жϸ�Ƶ�ʽڵ����Ƿ���Node�ڵ���ڣ�Ϊ����֮��ɾ��
	bool isNull() {
		if (headNode->next == tailNode) {
			return true;
		}
		return false;
	}

};

//myLFUCache LFU�������ʹ�û����㷨�࣬
//��ʾ������̭���ʹ��Ƶ�����ٵ����ݣ�����ʹ��Ƶ�����ٵĽڵ���ѡ��������ʵĽڵ���̭
class myLFUCache {
	//capacity��������
	//size�ѻ����С
	//map��¼�ڵ���keyֵ��������O(1)ʱ����ȷ��key�Ƿ����
	//headΪƵ�ʽڵ�ͷָ�룬tailΪƵ�ʽڵ�βָ��
	//Ƶ���ɸߵ���Ϊ��ͷָ�뵽βָ��
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
	//ɾ��һ���ڵ㣬����ɾ���ڵ�
	Node* deleteNode(Node* node) {
		node->next->pre = node->pre;
		node->pre->next = node->next;
		return node;
	}
	//ɾ��һ��Ƶ�ʽڵ㣬���ظ�Ƶ�ʽڵ����һ��Ƶ�ʽڵ�
	freNode* deleteFreNode(freNode* node) {
		freNode* nextFreNode = node->next;
		node->next->pre = node->pre;
		node->pre->next = node->next;
		return nextFreNode;
	}
	//��nextFreNodeǰ���½�һ��fre=freq��Ƶ�ʽڵ㣬���ظ�Ƶ�ʽڵ�
	freNode* newFreNodeInList(int freq, freNode* nextFreNode) {
		freNode* node = new freNode(freq);
		freNode* tem = nextFreNode->pre;
		nextFreNode->pre = node;
		tem->next = node;
		node->pre = tem;
		node->next = nextFreNode;
		return node;
	}
	//��headͷ�������½�һ��fre=freq��Ƶ�ʽڵ㣬���ظ�Ƶ�ʽڵ�
	freNode* newFreNodeHead(int freq) {
		freNode* node = new freNode(freq);
		freNode* tem = head->next;
		head->next = node;
		tem->pre = node;
		node->pre = head;
		node->next = tem;
		return node;
	}
	//��node�ڵ����freqNodeƵ�ʽڵ��������ͷ���headNode����
	void insertFreNode(Node* node, freNode* freqNode) {
		Node* tem = freqNode->headNode->next;
		freqNode->headNode->next = node;
		tem->pre = node;
		node->pre = freqNode->headNode;
		node->next = tem;
	}
	//��node�ڵ����freq+1��Ƶ�ʽڵ���
	void addFre(Node*& node) {
		//���жϸýڵ�����Ƶ�ʽڵ��Ƿ�Ϊ��
		//��Ϊ���ˣ���ɾ����Ƶ�ʽڵ�
		//�鿴��Ƶ�ʽڵ���һ���ڵ��Ƿ�Ϊfreq + 1
		//���ڵ����freq+1��Ƶ�ʽڵ���
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
		//����Ϊ��
		//�鿴��Ƶ�ʽڵ���һ���ڵ��Ƿ�Ϊfreq + 1
		//���ڵ����freq+1��Ƶ�ʽڵ���
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
	//����node�ڵ����fre=1��Ƶ�ʽڵ���
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
	//�õ�key��ֵ��ֵ����������ü�ֵ�ķ���Ƶ�μ�1
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
	//���루key,value���ڵ�
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
				//ע�ⲻ��Ҫsize++;
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
				//�ǵ�Ҫɾ��map����̭�Ĺؼ���
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
	myLFUCache cache = myLFUCache(2 /* capacity (��������) */);

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