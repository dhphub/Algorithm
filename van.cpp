#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <numeric>

using namespace std;


void getVariance(const vector<vector<double> >data, vector<double>& variance) {

    vector<double> _0Data;  //第一个行向量
    vector<double> iData;   //中间第i行向量
    int varCount;           //方差个数，即每个行向量中三维点的个数

    _0Data = data.at(0);

    if(_0Data.size()%3 != 0) {
        cout<<"Error input"<<endl;
        exit(-1);
    } else {
        varCount = _0Data.size() / 3;
        cout<<"line vectors number is: "<<data.size()<<endl;
        cout<<"3d points number is: "<<varCount<<endl;
    }

    for(int i=0; i<varCount; i++) {
        vector<double> dis; //欧式距离向量
        for(int j=1; j<data.size(); j++) {
            iData = data.at(j);
            double x = iData.at(0+3*i);
            double y = iData.at(1+3*i);
            double z = iData.at(2+3*i);
            cout<<x<<" "<<y<<" "<<z<<endl;

            double x0 = _0Data.at(0+3*i);
            double y0 = _0Data.at(1+3*i);
            double z0 = _0Data.at(2+3*i);
            cout<<x0<<" "<<y0<<" "<<z0<<endl;

            double disSquareSum = (x-x0)*(x-x0) + (y-y0)*(y-y0) + (z-z0)*(z-z0); //距离平方和
            if(disSquareSum < 0) {
                cout<<"Error: sum of squares less than 0 in "<<j<<","<<i<<endl;
                 exit(-1);
            }
            dis.push_back(sqrt(disSquareSum));
        }

        double sum = accumulate(dis.begin(), dis.end(), 0.0);  //和
        double mean = sum / dis.size();   //平均值

        double sumTemp = 0.0;
        for(int k=0; k<dis.size(); k++) {
            sumTemp += (dis.at(k)-mean)*(dis.at(k)-mean);
        }
        double varTemp = sumTemp / dis.size();

        variance.push_back(varTemp);
    }
}

int main(int argc,char** argv) {
    vector<vector<double> > data;
    vector<double> _0line(9, 0.0);
    vector<double> _1line(3, 1.0);
    vector<double> _2line(3, 2.0);
    vector<double> _3line(3, 3.0);

    _1line.push_back(2.0);
    _1line.push_back(2.0);
    _1line.push_back(2.0);
    _1line.push_back(3.0);
    _1line.push_back(3.0);
    _1line.push_back(3.0);

    _2line.push_back(3.0);
    _2line.push_back(3.0);
    _2line.push_back(3.0);
    _2line.push_back(4.0);
    _2line.push_back(4.0);
    _2line.push_back(4.0);

    _3line.push_back(4.0);
    _3line.push_back(4.0);
    _3line.push_back(4.0);
    _3line.push_back(6.0);
    _3line.push_back(6.0);
    _3line.push_back(6.0);


    data.push_back(_0line);
    data.push_back(_1line);
    data.push_back(_2line);
    data.push_back(_3line);

    vector<double> variance;
    getVariance(data, variance);

    for(int i=0; i<variance.size(); i++) {
        cout<<variance.at(i)<<" ";
    }
    cout<<endl;
    return 0;

}

