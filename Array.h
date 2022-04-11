#include <iostream>
using namespace std;
#include<ctime>
class Array {
public:
    //属性
    unsigned char* data;//数据
    int sum;//包含数据总个数
    int index;//当前data位置
    int shape[16] = { 0 };//每个维度的长度
    int axisNum;//维数
    int nowAxis;//当前维度
    int isDelete;//析构时是否释放data的内存空间
    
    //析构函数
    ~Array() {
        if (isDelete == 1) {
            delete[]data;
        }
    }

    //复制构造函数
    Array(const Array& t) {
        isDelete = t.isDelete;
        index = t.index;
        sum = t.sum;
        axisNum = t.axisNum;
        nowAxis = t.nowAxis;
        memcpy(shape, t.shape, 16 * sizeof(int));
        //isDelete=0则共用内存空间
        if (isDelete == 0) {
            data = t.data;
        }
        else {
            data = new(nothrow)unsigned char[sum];
            if (data == nullptr) {
                cout << "Array申请内存空间失败！" << endl;
                exit(1);
            }
            memcpy(data, t.data, sum * sizeof(unsigned char));
        }
    }

    //构造函数
    template<typename ...Args>
    Array(Args... args) {
        // 获取参数包大小并转换为size_t数组
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Array维数超出限制！" << endl;
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
            cout << "Array申请内存空间失败！" << endl;
            exit(1);
        }
        
    }

    //at操作
    template<typename ...Args>
    Array at(Args... args) {
        // 获取参数包大小并转换为size_t数组
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Array维数超出限制！" << endl;
            exit(1);
        }
        //通过重载后的[]运算符实现
        size_t list[16] = { args... };
        Array t = (*this)[list[0]][list[1]][list[2]][list[3]][list[4]][list[5]][list[6]][list[7]][list[8]][list[8]][list[10]][list[11]][list[12]][list[13]][list[14]][list[15]];
        return t;
    }

    //reshape操作
    template<typename ...Args>
    void reshape(Args... args) { //错误检查
        // 获取参数包大小并转换为size_t数组
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Array维数超出限制！" << endl;
            exit(1);
        }
        size_t list[16] = { args... };
        size_t sum_old = 1, sum_new = 1;//原来的元素总数和新的元素总数
        for (int i = 0; i < num; i++) {
            sum_new *= list[i];
        }
        for (int i = 0; i < nowAxis; i++) {
            sum_old *= shape[i];
        }
        //如果原来的和新的元素总数不相同则出错
        if (sum_old != sum_new) {
            cout << "reshape错误！" << endl;
            exit(1);
        }
        //相同则更新维数信息
        axisNum -= nowAxis;
        nowAxis = num;
        axisNum += nowAxis;
        for (int i = 0; i < num; i++) {
            shape[i] = list[i];
        }
    }

    //返回data指针
    unsigned char* get_content() {
        return data;
    }

    //设置当前索引位置的值
    void set(unsigned char value) {
        this->data[index] = value;
    }

    //重载=
    Array& operator=(const Array& t) {
        //如果isDelete=1则先释放内存空间
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
            cout << "Array申请内存空间失败！" << endl;
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
    
    //重载类型转换
    operator unsigned char() {
        return data[index];
    } 
    operator int() {
        return data[index];
    }

    //[]重载
    Array operator[](int index) {
        //判断是否越界
        if (index >= shape[0] || index < 0) {
            cout << "Array越界！" << endl;
            exit(1);
        }
        Array t;//没有参数时对象没有做初始化
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
        //当前维度小于1则直接返回
        if (t.nowAxis <= 0) {
            return t;
        }
        //否则降低一维返回子矩阵
        t.nowAxis -= 1;
        for (int i = 0; i < t.nowAxis; i++) {
            t.shape[i] = shape[i + 1];
        }
        //计算子矩阵新的index
        t.index = 1;
        for (int i = 0; i < t.nowAxis; i++) {
            t.index *= t.shape[i];
        }
        t.index *= index;
        t.index += this->index;
        return t;
    }
    
    //重载+-*/
    Array operator+(Array& b) {
        if (nowAxis != b.nowAxis || sum!=b.sum) {
            cout << "Array相加时不匹配！" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array相加时不匹配！" << endl;
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
            cout << "Array相加时不匹配！" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array相加时不匹配！" << endl;
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
            cout << "Array相加时不匹配！" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array相加时不匹配！" << endl;
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
            cout << "Array相加时不匹配！" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array相加时不匹配！" << endl;
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