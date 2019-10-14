#include<stdio.h>
#include<malloc.h>
/*
좌표압축(hash + sort)
세그먼트 트리
offline
------------------------
online 풀이는 dynamic seg tree (저는 모릅니다..)
*/
#define size 100003 // hash size
#define pnum 29 // 왜넣었는지 모르겠군요;;;; 없어도 대는 line입니다.
struct rain
{
	long long val;
};
struct node
{
	int idx;
	struct node* next;
};
struct query
{
	int qk;
	long long a, b;
};
struct query qlist[100010]; // query 저장용
struct node* f[size], *r[size];
struct rain rtable[400010], sorted[400010]; // struct rain이 아닌 long long으로 하셔도 됩니다......
long long* tree; // seg tree
int tidx, n, qn, len;
long long alist[100010], origin[100010];
void input();
unsigned int get_k(long long);
int search(int, long long);
void push(int, int);
void save(long long);
void emq();
struct node pop(int);
void msort(struct rain[], int, int);
void merge(struct rain[], int, int, int);
int bs(struct rain[], int, long long);
void update(int, int);
void doquery(int, int, int, int, int, int);
void q1(long long, long long, int);
void q2(long long, long long, int);
void q3(long long, long long, int, int, int);
void q4(long long, int, int, int);
long long sum(int, int, int, int, int);
int kth(int, int, int, long long);
void make_answer();
int main(void)
{
	make_answer();
	return 0;
}
void make_answer()
{
	int i, len, node, start, end, l, r, g, idx;
	input(); // 입력
	emq(); // 리스트들을 비워줍니다.
	msort(rtable, 0, tidx - 1); //정렬을 해줍니다.
	/*
	rtable을 정렬해줍니다.
	rtable에 저장되어잇는 수들은 전부 distinct합니다. (set 처럼 중복 안댐)
	그러나 탐색을 빨리 하기위해서 정렬을 해줍니다. (set처럼 정렬대게)
	이진검색을 하기 위해서 입니다.
	*/
	for (len = tidx, node = start = l = r = 1; r - l + 1 < len; l <<= 1, r <<= 1, r |= 1); // seg tree size
	tree = (long long*)calloc(r + 1, sizeof(long long));//seg 할당
	for (g = l - 1, end = r - l + 1, i = 1; i <= n; i++)
	{
		//그럼 이제 offline으로 문제를 푸니까 초기입력을 seg tree에 반영을 해줍니다.
		/*
		 rtable에서 origin[i](최초 i번 지역의 쌓여있는 적설량)를 이진검색해서 idx를 받아 냅니다.
		 그러면 제가 146 line에서 설명한대로 좌표압축을 했지요?

		 origin[i]는 idx로 치환이 대고
		 seg tree leaf node의 정의인 눈이 origin[i]mm쌓인 도시의 갯수는
		 눈이 idxmm 쌓인 도시의 갯수로 치환이 됩니다.
		*/
		idx = bs(rtable, tidx, origin[i]);
		//그래서 idx를 기준으로 seg tree를 update 합니다.
		update(g + idx + 1, 1); // seg를 1base로 했어요.. rtable은 0base이라서 + 1 해야 합니다.....
	}
	//이제 쿼리를 실행해 봅시다.
	doquery(node, start, end, l, r, g);
	free(tree);//해제
	/*
	문제 다 풀고 든 생각

	굳이 hash를 사용할 필요가 없습니다
	모든 데이터를 저장한뒤 정렬하고 이진검색을 하면
	항상 같은 값에 대해서는 항상 같은 idx를 줄것이기 때문입니다.
	*/
}
void input()
{
	int i, k, s;
	/*
	하하;;; 제가 C를 사용하다보니 set이 없습니다.
	그래서 간단한 해싱을 구현했습니다. -> 사실 없어도 되는 부분입니다(차후 설명)
	*/
	scanf("%d %d", &n, &qn); // 도시수 쿼리수
	for (i = 1, tidx = 0; i <= n; i++)
	{
		scanf("%lld", &alist[i]); // alist -> area_list로 생각하시면 편할듯 합니다.
		origin[i] = alist[i];//오프라인으로 문제를 풀 것이기 때문에 origin배열에 저장을 우선 해두어야 합니다.
		k = get_k(alist[i]); // hash key를 받습니다.
		s = search(k, alist[i]); // key와 value를 사용해서 현재 hash table에 value가 저장대어 있는지 아닌지 검색합니다.
		if (s == -1) // 만약 hash table에 value가 없다면 저장을 합니다.
		{
			push(k, tidx); // chaining hash
			save(alist[i]); // 이 함수의 정의로 가보면
		}
	}
	//이제 도시마다 초기 눈의 적설량을 저장을 했고 
	//쿼리들을 저장해 둡시다.
	for (i = 0; i < qn; i++)
	{
		scanf("%d", &qlist[i].qk); // 쿼리종류
		if (qlist[i].qk == 4) // 4번 쿼리라면?
		{
			//하나 저장
			scanf("%lld", &qlist[i].a);
		}
		else // 1, 2, 3번 쿼리는 2개 저장
		{
			scanf("%lld %lld", &qlist[i].a, &qlist[i].b);
			if (qlist[i].qk == 1) // 1번 쿼리라면?
			{
				//지역 qlist[i].a에 눈이 qlist[i].bmm쌓인다. 입니다.
				alist[qlist[i].a] += qlist[i].b; // area_list[qlist[i].a] += qlist[i].b; 반영해줍니다.
				k = get_k(alist[qlist[i].a]);// 그러면 area_list[qlist[i].a]의 값에대한 key를 받습니다.
				s = search(k, alist[qlist[i].a]); // hash table 탐색
				if (s == -1) // 없으면
				{
					//저장
					/*
					PPT를 확인해 주세요 ㅠㅠ
					글로 쓸려니 필력이 너무 없네요 ㅠㅠㅠㅠㅠ
					*/
					push(k, tidx);
					save(alist[qlist[i].a]);
				}
				/*
				왜 이렇게 하나면 3번 4번 쿼리 문입니다.

				세그먼트 트리의 리프노드의 정의를 이렇게 합시다.
				-> kmm의 눈이 쌓이 도시의 갯수

				그러나 mm의 범위가 0 ~ 1000000000000000000(10 ^ 18) 이라서 세그먼트 트리 구성을 못합니다.
				그러면 우리는 좌표압축을 해야합니다.

				처음 입력으로 들어오는 n개의 강수량
				+
				쿼리로 만들어지는 강수량 = 쿼리갯수 * 2
				=
				n + qn * 2 = 300000 (최악일때)  -> 리프노드의 개수

				이는 충분히 세그먼트 트리를 구성할수 있습니다.

				*/
			}
			else if (qlist[i].qk == 2)
			{
				//지역 qlist[i].a에 눈이 qlist[i].bmm녹는다. 입니다.
				alist[qlist[i].a] -= qlist[i].b;
				k = get_k(alist[qlist[i].a]);
				s = search(k, alist[qlist[i].a]);
				if (s == -1)
				{
					push(k, tidx);
					save(alist[qlist[i].a]);
				}
			}
			else
			{
				// 3번쿼리 
				//현재 적설량이 qlist[i].a mm이상 qlist[i].b mm이하인 구역의 수를 센다.
				k = get_k(qlist[i].a);
				s = search(k, qlist[i].a);
				if (s == -1)
				{
					push(k, tidx);
					save(qlist[i].a);
				}
				k = get_k(qlist[i].b);
				s = search(k, qlist[i].b);
				if (s == -1)
				{
					push(k, tidx);
					save(qlist[i].b);
				}
				//마찬가지로 hash table을 탐색한 후 저장합니다.
			}
		}
	}
}
unsigned int get_k(long long val)
{
	//그냥 hash_size로 나누 나머지를 key값으로 했습니다.
	return val % size;
	//hash 저격당한다면... 그때 hash func 다시 바꿀게요.....
}
int search(int k, long long val)
{
	//chaining 으로 hash 구성을 했기에 key값을 따라 동일한 값이 rtable에 저장이 대었는지 확인합니다.
	struct node* temp;
	for (temp = f[k]; temp; temp = temp->next)
	{
		if (rtable[temp->idx].val == val)
		{
			return temp->idx;
		}
	}
	return -1;
}
void push(int k, int idx)
{
	//key와 rtable의 index
	//list push_back 사실 큐라고 보시면 됩니다.
	struct node* temp;
	temp = (struct node*)malloc(sizeof(struct node));
	temp->idx = idx;
	temp->next = 0;
	if (!f[k])
	{
		f[k] = temp;
	}
	else
	{
		r[k]->next = temp;
	}
	r[k] = temp;
}
void save(long long val)
{
	//rtable이라는 배열에 그냥 저장하는 간단한 함수입니다.
	//여기에는 set처럼 중복대지 않는 숫자만 저장이 됩니다.
	//이유는 hash table에서 이미 저장대지 않은 value만 저장하기 때문이죠
	rtable[tidx++].val = val;
}
void emq()
{
	//list 비우기
	int i;
	for (i = 0; i < size; i++)
	{
		while (f[i])
		{
			pop(i);
		}
	}
}
struct node pop(int k)
{
	//list front pop 사실 큐라고 보셔도 무방합니다.
	struct node* temp, ret;
	temp = f[k];
	ret = *f[k];
	f[k] = f[k]->next;
	free(temp);
	return ret;
}
//merge sort입니다. 생략
void msort(struct rain list[], int left, int right)
{
	int mid;
	if (left < right)
	{
		mid = (left + right) >> 1;
		msort(list, left, mid);
		msort(list, mid + 1, right);
		merge(list, left, mid, right);
	}
}
//merge 입니다. 생략
void merge(struct rain list[], int left, int mid, int right)
{
	int i, j, k, l;
	k = i = left;
	j = mid + 1;
	while (i <= mid && j <= right)
	{
		if (list[i].val < list[j].val)
		{
			sorted[k++] = list[i++];
		}
		else
		{
			sorted[k++] = list[j++];
		}
	}
	if (mid < i)
	{
		for (l = j; l <= right; l++)
		{
			sorted[k++] = list[l];
		}
	}
	else
	{
		for (l = i; l <= mid; l++)
		{
			sorted[k++] = list[l];
		}
	}
	for (l = left; l <= right; l++)
	{
		list[l] = sorted[l];
	}
}
//이진 검색. 생략
int bs(struct rain list[], int limit, long long val)
{
	int left, mid, right;
	left = 0;
	right = limit - 1;
	while (left <= right)
	{
		mid = (left + right) >> 1;
		if (list[mid].val == val)
		{
			return mid;
		}
		if (list[mid].val < val)
		{
			left = mid + 1;
		}
		else
		{
			right = mid - 1;
		}
	}
}
//seg update입니다.
void update(int idx, int val)
{
	while (idx)
	{
		tree[idx] += val;
		idx >>= 1;
	}
}
void doquery(int node, int start, int end, int l, int r, int g)
{
	int i;
	for (i = 0; i < qn; i++)
	{
		switch (qlist[i].qk)
		{
		case 1: //1번쿼리
			q1(qlist[i].a, qlist[i].b, g);
			break;
		case 2: //2번쿼리
			q2(qlist[i].a, qlist[i].b, g);
			break;
		case 3: //3번쿼리
			q3(qlist[i].a, qlist[i].b, node, start, end);
			break;
		case 4: //4번쿼리
			q4(qlist[i].a, node, start, end);
			break;
		}
	}
}void q1(long long a, long long b, int g) // a번 지역의 눈이 b만큼 쌓인다.
{
	int idx;
	//origin[a]를 idx로 치환
	idx = bs(rtable, tidx, origin[a]);
	//seg tree를 idx에 따라 update - > 눈이 idx만큼 쌓인 도시의 갯수가 -1 됨
	update(g + idx + 1, -1);
	//a 지역에 b만큼 눈이 쌓이니 +=b 연산
	origin[a] += b;
	//origin[a]의 값이 바뀌었으니 다시 idx를 받아온다.
	idx = bs(rtable, tidx, origin[a]);
	//눈이 idx만큼 쌓인 도시의 갯수가 +1 됨
	update(g + idx + 1, 1);
}
void q2(long long a, long long b, int g) // a번 지역의 눈이 b만큼 녹는다.
{
	int idx;
	// q1함수를 참고
	idx = bs(rtable, tidx, origin[a]);
	update(g + idx + 1, -1);
	origin[a] -= b; // 여기만 바뀝니다!!!!!
	idx = bs(rtable, tidx, origin[a]);
	update(g + idx + 1, 1);
}
void q3(long long a, long long b, int node, int start, int end) // 눈이 amm ~ bmm 만큼 쌓인 도시의 갯수.
{
	int from_idx, to_idx;
	long long ans;
	//a 를 from_idx로 치환
	from_idx = bs(rtable, tidx, a);
	//b 를 to_idx로 치환
	to_idx = bs(rtable, tidx, b);
	//seg tree leaf node의 정의가 kmm쌓인 도시의 갯수이니
	//seg tree에서 from_idx ~ to_idx사이의 합을 구한다.
	ans = sum(node, start, end, from_idx + 1, to_idx + 1);
	printf("%lld\n", ans);
}
//seg sum함수
long long sum(int node, int start, int end, int left, int right)
{
	if (left > end || right < start)
	{
		return 0;
	}
	if (left <= start && end <= right)
	{
		return tree[node];
	}
	return sum(node << 1, start, (start + end) >> 1, left, right) + sum((node << 1) | 1, ((start + end) >> 1) + 1, end, left, right);
}
void q4(long long a, int node, int start, int end) //현재 적설량이 a번째로 많은 도시에 쌓인 눈이 몇 mm인지 센다.
{
	int idx;
	//적설량이 k번째로 많은 도시의 적설량을 수하는 쿼리
	/*
	k번째로 많은 -> n - k 번째인 도시의 적설량!!!
	그러면 우리는 'n - k 번째인 도시의 적설량'을 나타내는 seg tree leaf node의 idx를 구하면 됩니다.
	*/
	idx = kth(node, start, end, n - a + 1);
	/*
	그럼 idx를 구했으면 다시 치환해야 합니다.
	지금껏 kmm의 적설량을 leaf node의 인덱스로 치환했으니

	지금은 leaf node의 idx를 kmm의 적설량으로 치환해 출력하면 됩니다.
	*/
	printf("%lld\n", rtable[idx - 1].val); // 이렇게요.
}
//segment tree kth func
int kth(int node, int start, int end, long long val)
{
	if (start == end) // idx를 찾음
	{
		return end;
	}
	//반반씩 나누어서 생각하면
	if (tree[node << 1] >= val) // 처음부터 중간 까지 (왼쪽 반) 의 합이 val 보다 크면 다시 왼편을 봐야합니다.. 코드보단 그림을 그려보시면 이해가 쉽습니다.........
	{
		return kth(node << 1, start, (start + end) >> 1, val);
	}
	return kth((node << 1) | 1, ((start + end) >> 1) + 1, end, val - tree[node << 1]);  // 중간 다음부터 끝 까지 (오른쪽 반)
}
