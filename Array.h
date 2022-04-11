#include <iostream>
using namespace std;
#include<ctime>
class Array {
public:
    //����
    unsigned char* data;//����
    int sum;//���������ܸ���
    int index;//��ǰdataλ��
    int shape[16] = { 0 };//ÿ��ά�ȵĳ���
    int axisNum;//ά��
    int nowAxis;//��ǰά��
    int isDelete;//����ʱ�Ƿ��ͷ�data���ڴ�ռ�
    
    //��������
    ~Array() {
        if (isDelete == 1) {
            delete[]data;
        }
    }

    //���ƹ��캯��
    Array(const Array& t) {
        isDelete = t.isDelete;
        index = t.index;
        sum = t.sum;
        axisNum = t.axisNum;
        nowAxis = t.nowAxis;
        memcpy(shape, t.shape, 16 * sizeof(int));
        //isDelete=0�����ڴ�ռ�
        if (isDelete == 0) {
            data = t.data;
        }
        else {
            data = new(nothrow)unsigned char[sum];
            if (data == nullptr) {
                cout << "Array�����ڴ�ռ�ʧ�ܣ�" << endl;
                exit(1);
            }
            memcpy(data, t.data, sum * sizeof(unsigned char));
        }
    }

    //���캯��
    template<typename ...Args>
    Array(Args... args) {
        // ��ȡ��������С��ת��Ϊsize_t����
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Arrayά���������ƣ�" << endl;
            exit(1);
        }
        size_t list[16] = { args... };
        axisNum = nowAxis = (int)num;
        isDelete = 1;
        index = 0;
        sum = 1;
        for (size_t i = 0; i < num; i++) {
            shape[i] = (int)list[i];
            sum *= (int)list[i];
        }
        if (num == 0) {
            return;
        }
        data = new(nothrow)unsigned char[sum];
        if (data == nullptr) {
            cout << "Array�����ڴ�ռ�ʧ�ܣ�" << endl;
            exit(1);
        }
        
    }

    //at����
    template<typename ...Args>
    Array at(Args... args) {
        // ��ȡ��������С��ת��Ϊsize_t����
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Arrayά���������ƣ�" << endl;
            exit(1);
        }
        //ͨ�����غ��[]�����ʵ��
        size_t list[16] = { args... };
        Array t = (*this)[list[0]][list[1]][list[2]][list[3]][list[4]][list[5]][list[6]][list[7]][list[8]][list[8]][list[10]][list[11]][list[12]][list[13]][list[14]][list[15]];
        return t;
    }

    //reshape����
    template<typename ...Args>
    void reshape(Args... args) { //������
        // ��ȡ��������С��ת��Ϊsize_t����
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Arrayά���������ƣ�" << endl;
            exit(1);
        }
        size_t list[16] = { args... };
        size_t sum_old = 1, sum_new = 1;//ԭ����Ԫ���������µ�Ԫ������
        for (int i = 0; i < num; i++) {
            sum_new *= list[i];
        }
        for (int i = 0; i < nowAxis; i++) {
            sum_old *= shape[i];
        }
        //���ԭ���ĺ��µ�Ԫ����������ͬ�����
        if (sum_old != sum_new) {
            cout << "reshape����" << endl;
            exit(1);
        }
        //��ͬ�����ά����Ϣ
        axisNum -= nowAxis;
        nowAxis = num;
        axisNum += nowAxis;
        for (int i = 0; i < num; i++) {
            shape[i] = list[i];
        }
    }

    //����dataָ��
    unsigned char* get_content() {
        return data;
    }

    //���õ�ǰ����λ�õ�ֵ
    void set(unsigned char value) {
        this->data[index] = value;
    }

    //����=
    Array& operator=(const Array& t) {
        //���isDelete=1�����ͷ��ڴ�ռ�
        if(isDelete==1){
            delete[]data;
        }
        isDelete = 1;
        index = 0;
        sum = t.sum;
        axisNum = nowAxis = t.nowAxis;
        memcpy(shape, t.shape, 16 * sizeof(int));
        data = new(nothrow)unsigned char[sum];
        if (data == nullptr) {
            cout << "Array�����ڴ�ռ�ʧ�ܣ�" << endl;
            exit(1);
        }
        memcpy(data, t.data + t.index, sum * sizeof(unsigned char));
        return *this;
    }
    Array& operator=(unsigned char data) {
        this->data[index] = data;
        return *this;
    }
    Array& operator=(int data) {
        this->data[index] = data;
        return *this;
    }
    
    //��������ת��
    operator unsigned char() {
        return data[index];
    } 
    operator int() {
        return data[index];
    }

    //[]����
    Array operator[](int index) {
        //�ж��Ƿ�Խ��
        if (index >= shape[0] || index < 0) {
            cout << "ArrayԽ�磡" << endl;
            exit(1);
        }
        Array t;//û�в���ʱ����û������ʼ��
        t.isDelete = 0;
        t.data = data;
        t.axisNum = axisNum;
        t.nowAxis = nowAxis;
        t.sum = sum;
        if (t.isDelete == 1) {
            t.isDelete = 0;
            delete[]t.data;
            t.data = data;
        }
        //��ǰά��С��1��ֱ�ӷ���
        if (t.nowAxis <= 0) {
            return t;
        }
        //���򽵵�һά�����Ӿ���
        t.nowAxis -= 1;
        for (int i = 0; i < t.nowAxis; i++) {
            t.shape[i] = shape[i + 1];
        }
        //�����Ӿ����µ�index
        t.index = 1;
        for (int i = 0; i < t.nowAxis; i++) {
            t.index *= t.shape[i];
        }
        t.index *= index;
        t.index += this->index;
        return t;
    }
    
    //����+-*/
    Array operator+(Array& b) {
        if (nowAxis != b.nowAxis || sum!=b.sum) {
            cout << "Array���ʱ��ƥ�䣡" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array���ʱ��ƥ�䣡" << endl;
                exit(1);
            }
        }
        Array c;
        c = *this;
        for (int i = 0; i < sum; i++) {
            c.data[i + c.index] += b.data[i + index];
        }
        return c;
    }

    Array operator-(Array& b) {
        if (nowAxis != b.nowAxis || sum != b.sum) {
            cout << "Array���ʱ��ƥ�䣡" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array���ʱ��ƥ�䣡" << endl;
                exit(1);
            }
        }
        Array c;
        c = *this;
        for (int i = 0; i < sum; i++) {
            c.data[i + c.index] -= b.data[i + index];
        }
        return c;
    }

    Array operator*(Array& b) {
        if (nowAxis != b.nowAxis || sum != b.sum) {
            cout << "Array���ʱ��ƥ�䣡" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array���ʱ��ƥ�䣡" << endl;
                exit(1);
            }
        }
        Array c;
        c = *this;
        for (int i = 0; i < sum; i++) {
            c.data[i + c.index] *= b.data[i + index];
        }
        return c;
    }

    Array operator/(Array& b) {
        if (nowAxis != b.nowAxis || sum != b.sum) {
            cout << "Array���ʱ��ƥ�䣡" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array���ʱ��ƥ�䣡" << endl;
                exit(1);
            }
        }
        Array c;
        c = *this;
        for (int i = 0; i < sum; i++) {
            c.data[i + c.index] /= b.data[i + index];
        }
        return c;
    }

    Array operator+(int b) {
        Array c;
        c = *this;
        for (int i = 0; i < sum; i++) {
            c.data[i + c.index] += b;
        }
        return c;
    }

    Array operator-(int b) {
        Array c;
        c = *this;
        for (int i = 0; i < sum; i++) {
            c.data[i + c.index] -= b;
        }
        return c;
    }

    Array operator*(int b) {
        Array c;
        c = *this;
        for (int i = 0; i < sum; i++) {
            c.data[i + c.index] *= b;
        }
        return c;
    }

    Array operator/(int b) {
        Array c;
        c = *this;
        for (int i = 0; i < sum; i++) {
            c.data[i + c.index] /= b;
        }
        return c;
    }
};