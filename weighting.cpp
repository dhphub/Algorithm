#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <numeric>

#define prevHJoint 6    //前 H 个关节
#define highWeightSum 0.7  //前 m 个元素的权重和
#define fragmentLength  10 //片段长度，即每个片段中的时间序列个数，即帧数

using namespace std;

//关节点的类

class Joint {
    public:
        Joint(int id, double energy, double weting) {   //默认构造函数
            jointID = id;
            jointEnergy = energy;
            jointWeighting = weting;
        }

        int getID() {
            return jointID;
        }

        double getEnergy() {
            return jointEnergy;
        }

        double getJointWeighting() {
            return jointWeighting;
        }

        void setID(int id) {
            jointID = id;
        }

        void setEnergy(double energy) {
            jointEnergy = energy;
        }

        void setWeighting(double weighting) {
            jointWeighting = weighting;
        }

    private:
        int jointID;  //关节ID
        double jointEnergy; //关节运动能量大小，即方差
        double jointWeighting; //关节所占的权重大小
};

void getWeighting(const vector<vector<double> >data, vector<double>& zz);
void getVariance(const vector<vector<double> >data, vector<double>& zz);  //从时间序列中得到每个关节点的运动能量大小，即方差
bool isExisted(int id, vector<int> orderID);

//参数data，里面的元素vector<doulbe> 存放的是某个时间序列所有关节点的数据
void getWeighting(const vector<vector<double> >data, vector<double>& weighting) {

    int keyCount;  //关节点个数
    keyCount = data.size();

    vector<double> iData; //第 i 帧的数据，包含所有关节点的三维信息
    vector<vector<double> >nFrageData; //第 n 个片段的所有关节点时间序列数据
    vector<vector<Joint> >allFragDeOrder;  //所有片段按照运动能量大小降序排列的关节ID向量
    vector<vector<Joint> >prevHJointallFrag;  //所有片段前 H 个运动能量最大的关节
    vector<vector<vector<double> > > allFragAllJoint;  //所有片段所有关节点的时间序列数据

    //vector<Joint> asEnergyJoint;
    vector<Joint> asWeightingJoint;

    int fragmentCount;   //片段个数

    if( data.size()%fragmentLength == 0) {
        fragmentCount = data.size() / fragmentLength;
    } else {
        fragmentCount = data.size() / fragmentLength + 1;
    }

    //得到分组后所有片段的时间序列数据
    for(int i=0; i<fragmentCount; i++) {
        vector<vector<double> >inFragData;    //片段内从开始帧到结束帧的所有关节的时间序列数据
        for(int j=0; j<fragmentLength; j++) {
            inFragData.push_back(data.at(j+i*fragmentLength));
        }
        allFragAllJoint.push_back(inFragData);
    }

    //得到所有分组的按照关节点能量大小降序排序的
    for(int i=0; i<fragmentCount; i++) {
        nFrageData = allFragAllJoint.at(i);
        vector<double> variance;  //一个片段内所有关节的方差，用来衡量关节的运动能量
        getVariance(nFrageData, variance);
        vector<Joint> deOrderIjointID;
        //先将数据存入
        for(int j=0; j<variance.size(); j++) {
            Joint temp(1,0.0,0.0);
            temp.setID(j+1); //关节 ID 是以 1 开头进行排序的
            temp.setEnergy(variance.at(j));
            temp.setWeighting(0.0);
            deOrderIjointID.push_back(temp);
        }
        //按照能量大小进行降序排列,用冒泡的方法
        vector<int> trurDeOrderID;
        for(int j=0; j<deOrderIjointID.size()-1; j++) {
            for(int k=deOrderIjointID.size()-1; k>j+1; k--) {
                if(deOrderIjointID.at(k).getEnergy() <= deOrderIjointID.at(k-1).getEnergy()) {
                    swap(deOrderIjointID.at(k), deOrderIjointID.at(k-1));
                }
            }
        }
        allFragDeOrder.push_back(deOrderIjointID); //将各个片段按照运动能量大小降序排列的关节存入
    }

    //所有片段前 H 个运动能量最大的关节的向量
    for(int i=0; i<fragmentCount; i++) {
        vector<Joint> jointOrderTemp;  //第 i 个片段的按关节能量降序的关节向量
        jointOrderTemp = allFragDeOrder.at(i);
        vector<Joint> addToPrev;
        for(int j=0; j<prevHJoint; j++) {
            addToPrev.push_back(jointOrderTemp.at(j));
        }
        prevHJointallFrag.push_back(addToPrev);
    }

    //计算权重 Si
    int jointCount;  //关节个数
    iData = data.at(0);
    if(iData.size()%3 == 0) {
        jointCount = iData.size() / 3;
    } else {
        cout<<"error: input in 0 line."<<endl;
        exit(0);
    }


    //得到没有进行比例分配的权重,即 Si,所有权重存储在一个Joint的向量中
    for(int i=1; i<=jointCount; i++) {
        int iCount = 0;
        for(int j=0; j<prevHJointallFrag.size(); j++) {
            vector<Joint> jointTemp;
            jointTemp = prevHJointallFrag.at(j);
            for(int k=0; k<jointTemp.size(); k++) {
                Joint temp = jointTemp.at(k);
                if(temp.getID() == i) {
                    iCount += 1;
                }
            }
        }
        double beginWeighting = (double)iCount / (prevHJoint * fragmentCount);

        Joint temp(1, 0.0, 0.0);
        temp.setID(i);
        temp.setWeighting(beginWeighting);
        temp.setEnergy(0.0);
        asWeightingJoint.push_back(temp);
    }

    //按照JointEnergy的大小进行降序排列，
    for(int i=0; i<asWeightingJoint.size()-1; i++) {
        //Joint temp;
        //temp = asWeightingJoint.at(i);
        for(int j=0; j<asWeightingJoint.size()-1-i; j++) {
            if(asWeightingJoint.at(j).getJointWeighting() <= asWeightingJoint.at(j+1).getJointWeighting()) {
                swap(asWeightingJoint[j], asWeightingJoint[j+1]);
            }
        }
    }

    //得到前 m 个元素的集合　Ｃ　
    double weightSum = 0.0;
    int preHighCount = 0;
    vector<int> preID;
    for(int i=0; i<asWeightingJoint.size() && weightSum<=highWeightSum; i++) {
        Joint temp(1, 0.0, 0.0);
        temp = asWeightingJoint.at(i);
        weightSum += temp.getJointWeighting();
        preHighCount += 1;
        preID.push_back(temp.getID());
    }

    //得到最终的关节权重
    for(int i=0; i<jointCount; i++) {
        Joint temp(1, 0.0, 0.0);
        temp = asWeightingJoint.at(i);
        double finalWeighting;
        if(isExisted(temp.getID(), preID)) {
            finalWeighting = temp.getJointWeighting() * highWeightSum / weightSum;
        } else {
            finalWeighting = (jointCount - preHighCount) / (1 - highWeightSum);
        }
        asWeightingJoint[i].setWeighting(finalWeighting);
    }

    double tempWeighting;
    for(int i=1; i<=jointCount; i++) {
        for(int j=0; j<asWeightingJoint.size(); j++) {
            if (i == asWeightingJoint.at(j).getID()) {
                tempWeighting = asWeightingJoint.at(j).getJointWeighting();
            }
        }
        weighting.push_back(tempWeighting);
    }

    //对 Si 进行将序排列,得到 Si‘，然后取其前 m 其和个大于 highWeightSum，并将对应的ＩＤ送入集合Ｃ。
    /* for(int i=0; i<unRatioWeighting.size()-1; i++) {
        int index = 0;
        int tempCount = 0;
        for(int j=0; j<unRatioWeighting.size()-1-i; j++) {
           if(unRatioWeighting.at(j) <= unRatioWeighting.at(j+1)) {
                swap(unRatioWeighting.at(j), unRatioWeighting.at(j+1));
           }
           if(tempCount == 0) {
                index = j+1;  //保留最开始的索引，此处为关节ID
           }
           tempCount ++;
        }
    } */

}

bool isExisted(int id, vector<int> orderID) {
    for(int i=0; i<orderID.size(); i++) {
        if(id == orderID.at(i))
            return true;
    }
    return false;
}

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
            iData = data.at(j);  //第j行向量
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

        double sum = accumulate(dis.begin(), dis.end(), 0.0);  //距离和
        double mean = sum / dis.size();   //距离的平均值

        double sumTemp = 0.0;  //距离与均值的平方和
        for(int k=0; k<dis.size(); k++) {
            sumTemp += (dis.at(k)-mean)*(dis.at(k)-mean);
        }
        double varTemp = sumTemp / dis.size(); //距离的方差

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

