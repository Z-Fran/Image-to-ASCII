#include <iostream>
using namespace std;
#include<ctime>
class Array {
public:
    //Data
    unsigned char* data;//Data pointer
    int sum;//Total number of elements
    int index;//Current data position
    int shape[16] = { 0 };//Length of each dimension
    int axisNum;//Number of dimensions
    int nowAxis;//Current dimension
    int isDelete;//Whether to release data memory space when destructing
    
    //Destructor
    ~Array() {
        if (isDelete == 1) {
            delete[]data;
        }
    }

    //Copy constructor
    Array(const Array& t) {
        isDelete = t.isDelete;
        index = t.index;
        sum = t.sum;
        axisNum = t.axisNum;
        nowAxis = t.nowAxis;
        memcpy(shape, t.shape, 16 * sizeof(int));
        //isDelete=0, no memory space
        if (isDelete == 0) {
            data = t.data;
        }
        else {
            data = new(nothrow)unsigned char[sum];
            if (data == nullptr) {
                cout << "Array memory allocation failed!" << endl;
                exit(1);
            }
            memcpy(data, t.data, sum * sizeof(unsigned char));
        }
    }

    //Constructor
    template<typename ...Args>
    Array(Args... args) {
        // Get the number of parameters and convert to size_t type
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Array dimension exceeds limit!" << endl;
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
            cout << "Array memory allocation failed!" << endl;
            exit(1);
        }
        
    }

    //at function
    template<typename ...Args>
    Array at(Args... args) {
        // Get the number of parameters and convert to size_t type
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Array dimension exceeds limit!" << endl;
            exit(1);
        }
        //Implemented through operator overloading of []
        size_t list[16] = { args... };
        Array t = (*this)[list[0]][list[1]][list[2]][list[3]][list[4]][list[5]][list[6]][list[7]][list[8]][list[8]][list[10]][list[11]][list[12]][list[13]][list[14]][list[15]];
        return t;
    }

    //reshape function
    template<typename ...Args>
    void reshape(Args... args) { //Reshape
        // Get the number of parameters and convert to size_t type
        auto num = sizeof...(args);
        if (num > 16) {
            cout << "Array dimension exceeds limit!" << endl;
            exit(1);
        }
        size_t list[16] = { args... };
        size_t sum_old = 1, sum_new = 1;//Original number of elements and new number of elements
        for (int i = 0; i < num; i++) {
            sum_new *= list[i];
        }
        for (int i = 0; i < nowAxis; i++) {
            sum_old *= shape[i];
        }
        //If the original and new number of elements are different, error
        if (sum_old != sum_new) {
            cout << "reshape error" << endl;
            exit(1);
        }
        //Update dimension information
        axisNum -= nowAxis;
        nowAxis = num;
        axisNum += nowAxis;
        for (int i = 0; i < num; i++) {
            shape[i] = list[i];
        }
    }

    //Get data pointer
    unsigned char* get_content() {
        return data;
    }

    //Set value at current position
    void set(unsigned char value) {
        this->data[index] = value;
    }

    //Operator =
    Array& operator=(const Array& t) {
        //If isDelete=1, release memory space
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
            cout << "Array memory allocation failed!" << endl;
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
    
    //Type conversion
    operator unsigned char() {
        return data[index];
    } 
    operator int() {
        return data[index];
    }

    //[] operator
    Array operator[](int index) {
        //Check if out of bounds
        if (index >= shape[0] || index < 0) {
            cout << "Array out of bounds!" << endl;
            exit(1);
        }
        Array t;//No initialization when no parameters
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
        //If current dimension is less than 1, return directly
        if (t.nowAxis <= 0) {
            return t;
        }
        //Otherwise reduce one dimension and calculate submatrix
        t.nowAxis -= 1;
        for (int i = 0; i < t.nowAxis; i++) {
            t.shape[i] = shape[i + 1];
        }
        //Calculate new index for submatrix
        t.index = 1;
        for (int i = 0; i < t.nowAxis; i++) {
            t.index *= t.shape[i];
        }
        t.index *= index;
        t.index += this->index;
        return t;
    }
    
    //Operators +-*/
    Array operator+(Array& b) {
        if (nowAxis != b.nowAxis || sum!=b.sum) {
            cout << "Array operation mismatch!" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array operation mismatch!" << endl;
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
            cout << "Array operation mismatch!" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array operation mismatch!" << endl;
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
            cout << "Array operation mismatch!" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array operation mismatch!" << endl;
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
            cout << "Array operation mismatch!" << endl;
            exit(1);
        }
        for (int i = 0; i < nowAxis; i++) {
            if (shape[i] != b.shape[i]) {
                cout << "Array operation mismatch!" << endl;
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