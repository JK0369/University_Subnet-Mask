#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 프로그램에 앞서 scanf를 사용하기 때문에 전처리기에 _CRT_SECURE_NO_WARNING 추가한다.
// subnet_calculator
// 201352048 김종권

/* source program 순서

1. 디버그용 함수
1) get_size : bit수의 길이를 알아내는 함수
2) print_int_to_bit : integer값을 bit로 출력하는 함수

2. 전역 변수 선언 (키보드 입력에 따라 저장될 변수 위주)

3. 예외 처리 함수 (예외 메세지 출력)
1) 4개의 네트워크 ip를 못받는 경우
2) 네트워크 ip 주소 값이 0~255사이가 아닌 경우
3) 숫자가아닌 문자를 입력한 경우
4) 127로 시작하는 ip를 입력한 경우
5) 네트워크 ID를 잘못 입력한 경우(subnet mask와 비교)
6) 각 서브넷 당 호스트의 수를 모두 입력하지 않았거나 초과하여 입력한 경우

3. 버블정렬 구현 함수

4. 키보드 입력 관련 함수
1) my_split : 매개변수에서 받은 문자열 ip 주소를 split하여 정수로 바꾸는 함수
2) in : 키보드 입력을 그대로 받아서 변수에 저장 (이 함수에서 my_slpit호출, 내림차순 정렬까지 완료)

5. subnetting 관련 함수
1) search_min : (host 크기 + 2) 보다 큰 2의 제곱승 값 중에서, 최소 값인 몇 제곱승인지를 반환하는 함수 호출
2) store : 각 subnet당 필요한 host값(2의 n승) 저장하는 함수(전역변수인 host_requirement에 저장)
3) convert_to_32bit : 옥텟값을 받아서 32bit값으로 반환하는 함수
4) execute : subnetting 실행(반환값은 첫 번째 subnet의 ip min 값)
5) get_min_max : 첫 번째 서브넷의 할당가능한 최소값 ip를 받아서, 나머지 서브넷의 min,max값을 구하는 함수
6) convert_to_octet : 10진수를 입력받아, 4개의 ip 배열(옥텟)로 변환하여 반환하는 함수
7) convert : 각 subnet당 min과 max값을 옥텟 값으로 2차원 배열 반환 (여기서 convert_to_octet호출)
반환 값[i][j] : i=min, max, min, max,,, 반복(할당불가시 NULL), j=옥텟값의 인덱스
8) print_reult : 옥텟 값을 출력하는 함수

*/


// *For test //
// bit수의 길이를 알아내는 함수 
int get_size(long long int value) {
	int i = 0;
	while (value != 0 && value != -1) {
		i++;
		value = value >> 1;
	}
	return i;
}

// *For test //
// integer값을 bit로 출력하는 함수
void print_int_to_bit(long long int value) {

	if (value == -1) {
		printf("%lld = unused\n", value);
		return;
	}
	// bit 길이
	int size = get_size(value);

	printf("%lld = ", value);
	for (int i = size - 1; i >= 0; i--)
		printf("%lld", (value >> i) & 1);

	if (size == 0) printf("%d", 0);
	printf("(2)\n");
}


// subnetting에 필요한 변수
int * nw_id;	// 옥텟 ip
long long int IP; // 32bit ip
int subnet_mask;
int subnet_size;
int * host_size_each_subnet;
int * host_requirement; // 각 subnet에서 필요한 host의 수(2의 n제곱승에서 n의 값)
int is_ip_subnet = 0;
#define IPV 32 // IPv4의 bit수

// IP 4개를 다 못받은 경우 예외처리
void exception_not_qualify_size() {
	printf("* exception : not qualify number of ip sizes \n\n");
	exit(1);
}

// 0~255사이의 ip주소값이 아닌 경우 예외처리
void exception_exceed_range() {
	printf("* exception : exceed the range (0~255) \n\n");
	exit(1);
}

// 자료형을 잘못 입력한 경우 예외처리
void exception_not_qualify_type() {
	printf("* exception : not qualify type \n\n");
	exit(1);
}

// 127로 시작하는 loopback address인 경우 예외처리
void exception_loopback_address() {
	printf("* exception : this n/w id is a loopback address \n\n");
	exit(1);
}

// 네트워크 id를 잘못 입력한 경우
void exception_nw_id() {
	printf("* exception : intput n/w id is not network id \n");
	printf("but, program keep going by regarding that input n/w id is IP address.\n\n");
}

// 네트워크 id를 잘못 입력한지 판단하는 함수
int is_exception_nw_id(int sm) {
	// sm : subnet mask, in_nw_id : 입력받은 네트워크 id
	long long int tmp = 1;
	long long int not_SM = (1 << (IPV - sm)) - 1;

	// 네트워크 id를 subnet_mast와 비교했을 때 잘못 입력한 경우 1 / 잘 입력한 경우 0
	if ((IP&not_SM) == 0) return 0;
	else return 1;
}

// 각 서브넷 당 호스트의 수를 모두 입력하지 않았거나 초과하여 입력한 경우
void exception_not_qualify_host_size() {
	printf("* exception : not qualify number of host sizes \n\n");
	exit(1);
}

// 버블 정렬 (내림차순 정렬에 사용됨(subnet 당 host 수, 배열에 사용)
void bubble_sort(int * arr, int n) {
	int i, j, tmp;
	for (i = n - 1; i>0; i--)
		for (j = 0; j<i; j++)
			if (arr[j]<arr[j + 1]) {
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}
}

// 입력 받은 문자열 ip 주소를 split하여 정수로 바꾸는 함수
int * my_split(char * str, char * token, int subnet_size, int is_nw_id) {
	char * ptr;
	int index_size;

	// 네트워크 id에 대해 작업할 것인지, Subnet 당 호스트 수에 대해 작업할 것인지 구별
	if (is_nw_id) index_size = 4; // nw_id인경우
	else index_size = subnet_size; // subnet당 호스트 수인 경우

	int * result_split = (int*)malloc(sizeof(int) * index_size);

	ptr = strtok(str, token);
	if (ptr == NULL) exception_not_qualify_size();

	result_split[0] = atoi(ptr); // ascii to Int
	for (int i = 1; i < index_size; i++) {
		ptr = strtok(NULL, token);

		// 각 서브넷당 호스트의 수를 모두 입력하지 않은 경우 예외처리
		if (ptr == NULL) exception_not_qualify_host_size();
		result_split[i] = atoi(ptr);
	}

	// ip주소에 대해 작업하는 경우, 범위(0~255)를 넘어서면 예외처리
	if (is_nw_id) {
		for (int i = 0; i < index_size; i++) {
			// 0~255사이의 ip주소값이 아닌 경우 예외처리
			if (result_split[i] > 255 || result_split[i] < 0) exception_exceed_range();
		}

		if (result_split[0] == 127) exception_loopback_address();
	}
	else { // 각 서브넷당 호스트의 수를 초과로 입력한 경우 예외처리
		ptr = strtok(NULL, token);
		if (ptr != NULL) exception_not_qualify_host_size();
	}
	return result_split;
}

// 4개의 옥텟값을 받아서 32bit값으로 반환하는 함수
long long int convert_to_32bit(int * arr) {
	long long int tmp = 1;
	long long int ip = 0;

	int j = 24;
	for (int i = 0; i < 4; i++, j -= 8) {
		long long int arr_tmp = arr[i];
		ip += arr_tmp << j;
	}

	return ip;
}

#define MAX 123456
// 입력 함수 & 서브넷 당 호스트 크기는 내림차순 정렬
// 예외 처리도 동시에 진행
//* 1) IP주소의 값 중, 0~255 사이의 값이 아닌 경우
//* 2) 정수가 아닌 문자와 같은 다른 타입을 입력한 경우
//* 3) subnet mask 크기가 32초과인 경우
//* 4) 127의 IP주소를 갖는 것 : loopback address
//* 5) n/w id와 submask 비교했을 때 잘못 입력한 경우
void in() {
	char nw_id_[MAX] = "";
	subnet_mask = -1;
	subnet_size = -1;
	char host_size_each_subnet_[MAX] = "";

	printf("********************** input ********************************");
	printf("\n* 1. n/w id : "); gets(nw_id_);
	printf("\n* 2. subnet mask : "); scanf("%d", &subnet_mask);
	printf("\n* 3. number of subnet : "); scanf("%d", &subnet_size); gets();
	printf("\n* 4. number of host at each subnet : "); gets(host_size_each_subnet_);
	printf("********************** finish ********************************\n\n\n");

	// 자료형을 잘못 입력한 경우 예외처리
	if (subnet_mask == -1 || subnet_size == -1) exception_not_qualify_type();

	nw_id = my_split(nw_id_, ".", subnet_size, 1);
	host_size_each_subnet = my_split(host_size_each_subnet_, " ", subnet_size, 0);

	// 내림차순 정렬
	bubble_sort(host_size_each_subnet, subnet_size);

	// 32bit ip
	IP = convert_to_32bit(nw_id);

	// 네트워크 id를 subnet mask에 비교했을 때 잘못 입력한 경우 예외처리.
	// 단, 그런 경우 네트워크 id를 전체 ip로 보고 진행함.
	if (is_exception_nw_id(subnet_mask)) {
		exception_nw_id();
		is_ip_subnet = 1;
	}

}

// (host 크기 + 2) 보다 큰 2의 제곱승 값 중에서, 최소 값인 몇 제곱승인지를 반환하는 함수 호출
int search_min(int host_size) {
	int requirement = host_size + 2;
	int i = 0;
	for (i = 0;; i++) {
		if (requirement <= (1 << i))
			break;
	}
	return i;
}

// 각 subnet당 필요한 host값(2의 n승) 저장하는 함수
void store() {
	host_requirement = (int*)malloc(sizeof(int)*subnet_size);
	for (int i = 0; i < subnet_size; i++) {
		host_requirement[i] = search_min(host_size_each_subnet[i]);
	}
}

// subnetting 실행(반환값은 첫 번째 subnet의 ip min 값)
long long int execute() {
	long long int tmp = 1;
	int HB = host_requirement[0]; // host bit
	if (is_ip_subnet) HB++;
	long long int SM = ((tmp << IPV - HB) - 1) << HB;	// subnet mask
	long long int NA = IP & SM;		// network address, IP : 32bit
	return NA + 1;
}

// 첫 번째 서브넷의 할당가능한 최소값 ip를 받아서, 나머지 서브넷의 min,max값을 구하는 함수
long long int ** get_min_max(long long int first_subnet_ip_min) {

	// 반환값을 위한 동적 할당[i][j] : i는 서브넷 크기, j는 최소와 최대값
	long long int ** result = (long long int **)malloc(sizeof(long long int *)*subnet_size);
	for (int i = 0; i<subnet_size; i++)
		result[i] = (int*)malloc(sizeof(int) * 2);

	for (int i = 0; i < subnet_size; i++)
		for (int j = 0; j < 2; j++)
			result[i][j] = -1;

	result[0][0] = first_subnet_ip_min;
	result[0][1] = result[0][0] + (1 << host_requirement[0]) - 3;
	long long int capacity = 1 << (IPV - subnet_mask);
	capacity -= (1 << host_requirement[0]);

	// sum 값이 할당 가능한 host의 수보다 크게 되면 -1 할당
	for (int i = 1; i < subnet_size && capacity > 0; i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 0) result[i][0] = result[i - 1][1] + 3;
			else {
				int h_requirement = (1 << host_requirement[i]);
				result[i][1] = result[i][0] + h_requirement - 3;
				capacity -= h_requirement;
				if (capacity < 0) {
					result[i][1] = -1;
					result[i][0] = -1;
				}
			}
		}
	}
	return result;
}

// 10진수를 입력받아, 4개의 ip 배열(옥텟)로 변환하여 반환하는 함수
int * convert_to_octet(long long int value) {

	int * ip = (int*)malloc(sizeof(int) * 4);
	int j = 24;
	for (int i = 0; i < 4; i++, j -= 8) {
		ip[i] = (value >> j);
		ip[i] = ip[i] & ((1 << 8) - 1);
	}
	return ip;
}

// 옥텟 값으로 2차원 배열 반환
int ** convert(long long int ** subnet_ip) {

	// 반환값을 위한 동적 할당[i][j] : : i=min, max, min, max,,, 반복(할당불가시 NULL), j=옥텟값
	int length = 2 * subnet_size;
	int ** result = (int **)malloc(sizeof(int *)*length);
	for (int i = 0; i<length; i++)
		result[i] = (int*)malloc(sizeof(int) * 4);

	int q = 0;
	for (int i = 0; i < subnet_size; i++, q++) {
		if (subnet_ip[i][1] == -1) {
			result[i + q] = NULL;
			result[i + 1 + q] = NULL;
		}
		else {
			result[i + q] = convert_to_octet(subnet_ip[i][0]);
			result[i + 1 + q] = convert_to_octet(subnet_ip[i][1]);
		}
	}
	return result;
}

// 옥텟 값을 출력하는 함수
void print_reult(int ** value) {
	// value[i][j] : i=min, max, min, max,,, 반복(할당불가시 NULL), j=옥텟값

	printf("*************************************************************\n");

	int q = 0;
	for (int i = 0; i < subnet_size; i++, q++) {
		printf("%d)subnet : ", i + 1);
		if (value[i + q] == NULL) {
			printf("unused");
		}
		else {

			// MIN
			for (int j = 0; j < 4; j++) {
				if (j == 3) printf("%d", value[i + q][j]);
				else printf("%d.", value[i + q][j]);
			}

			// MAX
			printf(" ~ ");
			for (int j = 0; j < 4; j++) {
				if (j == 3) printf("%d(/%d)", value[i + 1 + q][j], (IPV - host_requirement[q]));
				else printf("%d.", value[i + 1 + q][j]);
			}
		}
		printf("\n");
	}
	printf("*************************************************************\n");
}

int main() {

	// 키보드 입력 받아, split을 이용해 전역 변수에 입력하는 작업
	in();

	// 각 subnet당 필요한 host값(2의 n승) 저장하는 작업
	store();

	// subnetting 실행(반환값은 첫 번째 subnet의 ip min 값)
	long long int first_subnet_ip_min = execute(subnet_size);

	;	// 나머지 subnet들 ip의 최소값, 최대값 반환하는 함수 (NULL값이 return 되면 할당 불가한 크기)
	long long int ** subnet_ip = get_min_max(first_subnet_ip_min); //[i][j] : i는 서브넷 크기, j는 최소와 최대값
	if (subnet_ip == NULL) printf("할당 불가한 크기");

	// 32비트를 옥텟으로 변환 하여 반환 하는 함수([i][j] : i=min,mix,min,max... / j=옥텟값)
	int ** result = convert(subnet_ip);

	// 형식화하여 출력하는 함수
	print_reult(result);

	return 0;
}