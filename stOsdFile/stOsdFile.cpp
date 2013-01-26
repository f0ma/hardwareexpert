#include "stOsdFile.h"

using namespace std;

QByteArray StOSDFile::importFile(QString filename)
{
//    cout <<"Process..."<< endl <<flush;

    QString inputFileName = filename;

    inputFileName = inputFileName.mid(0,inputFileName.length()-4);

    QFile fi0(inputFileName+".os0");
    QFile fi1(inputFileName+".os1");

    fi0.open(QIODevice::ReadOnly| QIODevice::Text);
    fi1.open(QIODevice::ReadOnly| QIODevice::Text);

    if(!fi0.isOpen() || !fi1.isOpen())
    {
        return QByteArray();
    }


    unsigned char osd0Hi[10][512];
    unsigned char osd0Lo[10][512];

    unsigned char osd1Hi[10][512];
    unsigned char osd1Lo[10][512];


//    cout <<"\tReading files...";

    int row_index = 0;

    while(!fi0.atEnd())
    {
        QString ds(fi0.readLine(256));

        string s = ds.toStdString();
        if (s.empty())continue;
        if (s[0]=='#')continue;

        for(int i=0;i<10;i++)
        {
            osd0Hi[i][row_index] = (unsigned char) strtol(s.substr(5*i,2).c_str(),NULL,16);
            osd0Lo[i][row_index] = (unsigned char) strtol(s.substr(5*i+2,2).c_str(),NULL,16);

           }

        row_index++;
        if(row_index == 512) break;
    }

    row_index = 0;

    while(!fi1.atEnd())
    {
        QString ds(fi1.readLine(256));

        string s = ds.toStdString();
        if (s.empty())continue;
        if (s[0]=='#')continue;

        for(int i=0;i<10;i++)
        {
            osd1Hi[i][row_index] = strtol(s.substr(5*i,2).c_str(),NULL,16);
            osd1Lo[i][row_index] = strtol(s.substr(5*i+2,2).c_str(),NULL,16);
        }
        row_index++;
        if(row_index == 512) break;
    }


    unsigned char hexHi[7680];
    unsigned char hexLo[7680];

    for(int i =0;i<7680;i++)
        for(int j =0;j<512;j++)
        {
            hexHi[i]=0xFF;
            hexLo[i]=0xFF;
        }

//    cout <<"OK"<<endl<<"\tEncoding...";

    for(int j=0;j<0x200;j++)
    {
        hexHi[j+0xA00*2] = osd0Lo[0][j]; // start 2 sector
        hexHi[j] = osd0Lo[1][j]; // start 0 sector
        hexHi[j+0xA00*2+0x200] = osd0Lo[2][j];
        hexHi[j+0x200] = osd0Lo[3][j];
        hexHi[j+0xA00*2+0x400] = osd0Lo[4][j];
        hexHi[j+0x400] = osd0Lo[5][j];
        hexHi[j+0xA00*2+0x600] = osd0Lo[6][j];
        hexHi[j+0x600] = osd0Lo[7][j];
        hexHi[j+0xA00*2+0x800] = osd0Lo[8][j];
        hexHi[j+0x800] = osd0Lo[9][j];


        hexLo[j+0xA00*2] = osd1Lo[0][j]; // start 2 sector
        hexLo[j] = osd1Lo[1][j]; // start 0 sector
        hexLo[j+0xA00*2+0x200] = osd1Lo[2][j];
        hexLo[j+0x200] = osd1Lo[3][j];
        hexLo[j+0xA00*2+0x400] = osd1Lo[4][j];
        hexLo[j+0x400] = osd1Lo[5][j];
        hexLo[j+0xA00*2+0x600] = osd1Lo[6][j];
        hexLo[j+0x600] = osd1Lo[7][j];
        hexLo[j+0xA00*2+0x800] = osd1Lo[8][j];
        hexLo[j+0x800] = osd1Lo[9][j];
    }


    for(int j=0;j<0x200;j++)
    {

        // 1
        hexHi[j+0xA00*1] = hexHi[j+0xA00*1] & ((osd0Hi[0][j] << 4) | 0b11001111);

        // 2
        hexHi[j+0xA00*1] = hexHi[j+0xA00*1] & ((osd0Hi[1][j] << 0) | 0b11111100) & ((osd0Hi[1][j] << 2) | 0b11110011) ;
        hexHi[j+0xA00*1+0x200] = hexHi[j+0xA00*1+0x200] & ((osd0Hi[1][j] << 2) | 0b11110011);

        // 3
        hexHi[j+0xA00*1] = hexHi[j+0xA00*1] & ((osd0Hi[2][j] << 6) | 0b00111111);
        hexHi[j+0xA00*1+0x200] = hexHi[j+0xA00*1+0x200] & ((osd0Hi[2][j] << 4) | 0b11001111);
        hexHi[j+0xA00*1+0x800] = hexHi[j+0xA00*1+0x800] & ((osd0Hi[2][j] << 6) | 0b00111111);

        //4
        hexHi[j+0xA00*1+0x200] = hexHi[j+0xA00*1+0x200] & ((osd0Hi[3][j] << 0) | 0b11111100);
        hexHi[j+0xA00*1+0x400] = hexHi[j+0xA00*1+0x400] & ((osd0Hi[3][j] << 2) | 0b11110011);


        //5
        hexHi[j+0xA00*1+0x200] = hexHi[j+0xA00*1+0x200] & ((osd0Hi[4][j] << 6) | 0b00111111);
        hexHi[j+0xA00*1+0x400] = hexHi[j+0xA00*1+0x400] & ((osd0Hi[4][j] << 4) | 0b11001111);

        //6
        hexHi[j+0xA00*1+0x400] = hexHi[j+0xA00*1+0x400] & ((osd0Hi[5][j] << 0) | 0b11111100);
        hexHi[j+0xA00*1+0x600] = hexHi[j+0xA00*1+0x600] & ((osd0Hi[5][j] << 2) | 0b11110011);
        //osd0Hi[5][j] = (hexHi[j+0xA00*1+0x400] & 0b00000011) >> 0;

        //7
        hexHi[j+0xA00*1+0x400] = hexHi[j+0xA00*1+0x400] & ((osd0Hi[6][j] << 6) | 0b00111111);
        hexHi[j+0xA00*1+0x600] = hexHi[j+0xA00*1+0x600] & ((osd0Hi[6][j] << 4) | 0b11001111);
        //osd0Hi[6][j] = (hexHi[j+0xA00*1+0x600] & 0b00110000) >> 4;

        //8
        hexHi[j+0xA00*1+0x600] = hexHi[j+0xA00*1+0x600] & ((osd0Hi[7][j] << 0) | 0b11111100);
        hexHi[j+0xA00*1+0x800] = hexHi[j+0xA00*1+0x800] & ((osd0Hi[7][j] << 2) | 0b11110011);
        //osd0Hi[7][j] = (hexHi[j+0xA00*1+0x600] & 0b00000011) >> 0;

        //9
        hexHi[j+0xA00*1+0x600] = hexHi[j+0xA00*1+0x600] & ((osd0Hi[8][j] << 6) | 0b00111111);
        hexHi[j+0xA00*1+0x800] = hexHi[j+0xA00*1+0x800] & ((osd0Hi[8][j] << 4) | 0b11001111);
        //osd0Hi[8][j] = (hexHi[j+0xA00*1+0x800] & 0b00110000) >> 4;

        //10
        hexHi[j+0xA00*1+0x800] = hexHi[j+0xA00*1+0x800] & ((osd0Hi[9][j] << 0) | 0b11111100);
        //osd0Hi[9][j] = (hexHi[j+0xA00*1+0x800] & 0b00000011) >> 0;
    }

    for(int j=0;j<0x200;j++)
    {

        // 1
        hexLo[j+0xA00*1] = hexLo[j+0xA00*1] & ((osd1Hi[0][j] << 4) | 0b11001111);

        // 2
        hexLo[j+0xA00*1] = hexLo[j+0xA00*1] & ((osd1Hi[1][j] << 0) | 0b11111100) & ((osd1Hi[1][j] << 2) | 0b11110011) ;
        hexLo[j+0xA00*1+0x200] = hexLo[j+0xA00*1+0x200] & ((osd1Hi[1][j] << 2) | 0b11110011);

        // 3
        hexLo[j+0xA00*1] = hexLo[j+0xA00*1] & ((osd1Hi[2][j] << 6) | 0b00111111);
        hexLo[j+0xA00*1+0x200] = hexLo[j+0xA00*1+0x200] & ((osd1Hi[2][j] << 4) | 0b11001111);
        hexLo[j+0xA00*1+0x800] = hexLo[j+0xA00*1+0x800] & ((osd1Hi[2][j] << 6) | 0b00111111);

        //4
        hexLo[j+0xA00*1+0x200] = hexLo[j+0xA00*1+0x200] & ((osd1Hi[3][j] << 0) | 0b11111100);
        hexLo[j+0xA00*1+0x400] = hexLo[j+0xA00*1+0x400] & ((osd1Hi[3][j] << 2) | 0b11110011);


        //5
        hexLo[j+0xA00*1+0x200] = hexLo[j+0xA00*1+0x200] & ((osd1Hi[4][j] << 6) | 0b00111111);
        hexLo[j+0xA00*1+0x400] = hexLo[j+0xA00*1+0x400] & ((osd1Hi[4][j] << 4) | 0b11001111);

        //6
        hexLo[j+0xA00*1+0x400] = hexLo[j+0xA00*1+0x400] & ((osd1Hi[5][j] << 0) | 0b11111100);
        hexLo[j+0xA00*1+0x600] = hexLo[j+0xA00*1+0x600] & ((osd1Hi[5][j] << 2) | 0b11110011);
        //osd0Hi[5][j] = (hexHi[j+0xA00*1+0x400] & 0b00000011) >> 0;

        //7
        hexLo[j+0xA00*1+0x400] = hexLo[j+0xA00*1+0x400] & ((osd1Hi[6][j] << 6) | 0b00111111);
        hexLo[j+0xA00*1+0x600] = hexLo[j+0xA00*1+0x600] & ((osd1Hi[6][j] << 4) | 0b11001111);
        //osd0Hi[6][j] = (hexHi[j+0xA00*1+0x600] & 0b00110000) >> 4;

        //8
        hexLo[j+0xA00*1+0x600] = hexLo[j+0xA00*1+0x600] & ((osd1Hi[7][j] << 0) | 0b11111100);
        hexLo[j+0xA00*1+0x800] = hexLo[j+0xA00*1+0x800] & ((osd1Hi[7][j] << 2) | 0b11110011);
        //osd0Hi[7][j] = (hexHi[j+0xA00*1+0x600] & 0b00000011) >> 0;

        //9
        hexLo[j+0xA00*1+0x600] = hexLo[j+0xA00*1+0x600] & ((osd1Hi[8][j] << 6) | 0b00111111);
        hexLo[j+0xA00*1+0x800] = hexLo[j+0xA00*1+0x800] & ((osd1Hi[8][j] << 4) | 0b11001111);
        //osd0Hi[8][j] = (hexHi[j+0xA00*1+0x800] & 0b00110000) >> 4;

        //10
        hexLo[j+0xA00*1+0x800] = hexLo[j+0xA00*1+0x800] & ((osd1Hi[9][j] << 0) | 0b11111100);
        //osd0Hi[9][j] = (hexHi[j+0xA00*1+0x800] & 0b00000011) >> 0;
    }


//    cout <<"OK"<<endl<<"\tSaving...";

//    intelhex hexFile;
    QByteArray data(0x5300,0x00);

    int addr =0;


    for(int i=0;i<7680;i=i+1)
            {

            data[addr]=hexHi[i];
            addr++;
            data[addr]=hexLo[i];
            addr++;

            if(addr>=0x1400 && addr <0x2000) {addr =0x2000; continue;}
            if(addr>=0x3400 && addr <0x4000) {addr =0x4000; continue;}

            }

    return data;
}

bool StOSDFile::exportFile(QString filename,QByteArray rdata)
{
    if (rdata.size()!=0x5400) return false;

    unsigned char hexHi[7680];
    unsigned char hexLo[7680];

    int index =0;
    bool p = 0;


    unsigned int addr=0;
    unsigned char data;

    while(addr<=0x53FF)
    {

    data = rdata[addr];

    if(addr>=0x1400 && addr <0x2000) {addr++; continue;}
    if(addr>=0x3400 && addr <0x4000){addr++; continue;}
    if(addr>=0x5400) {addr++; continue;}

    if(p==0)
    {
        hexHi[index] = data;
        p = 1;
    }
    else
    {
        hexLo[index] = data;
        p = 0;
        index++;
    }


    addr++;
    }

//    cout << index <<" cells readed" << endl;

    unsigned char osd0Hi[10][512];
    unsigned char osd0Lo[10][512];

    unsigned char osd1Hi[10][512];
    unsigned char osd1Lo[10][512];

    for(int i =0;i<10;i++)
        for(int j =0;j<512;j++)
        {
            osd0Hi[i][j] = 0x00;
            osd0Lo[i][j] = 0xFF;
            osd1Hi[i][j] = 0x00;
            osd1Lo[i][j] = 0xFF;
        }

//------------------------------------------------------------
    for(int j=0;j<0x200;j++)
    {
        osd0Lo[0][j] = hexHi[j+0xA00*2]; // start 2 sector
        osd0Lo[1][j] = hexHi[j]; // start 0 sector
        osd0Lo[2][j] = hexHi[j+0xA00*2+0x200];
        osd0Lo[3][j] = hexHi[j+0x200];
        osd0Lo[4][j] = hexHi[j+0xA00*2+0x400];
        osd0Lo[5][j] = hexHi[j+0x400];
        osd0Lo[6][j] = hexHi[j+0xA00*2+0x600];
        osd0Lo[7][j] = hexHi[j+0x600];
        osd0Lo[8][j] = hexHi[j+0xA00*2+0x800];
        osd0Lo[9][j] = hexHi[j+0x800];
    }


    for(int j=0;j<0x200;j++)
    {
        osd0Hi[0][j] = (hexHi[j+0xA00*1] & 0b00110000) >> 4; // start 1 sector
        osd0Hi[1][j] = (hexHi[j+0xA00*1] & 0b00000011) >> 0;
        osd0Hi[2][j] = (hexHi[j+0xA00*1+0x200] & 0b00110000) >> 4;
        osd0Hi[3][j] = (hexHi[j+0xA00*1+0x200] & 0b00000011) >> 0;
        osd0Hi[4][j] = (hexHi[j+0xA00*1+0x400] & 0b00110000) >> 4;
        osd0Hi[5][j] = (hexHi[j+0xA00*1+0x400] & 0b00000011) >> 0;
        osd0Hi[6][j] = (hexHi[j+0xA00*1+0x600] & 0b00110000) >> 4;
        osd0Hi[7][j] = (hexHi[j+0xA00*1+0x600] & 0b00000011) >> 0;
        osd0Hi[8][j] = (hexHi[j+0xA00*1+0x800] & 0b00110000) >> 4;
        osd0Hi[9][j] = (hexHi[j+0xA00*1+0x800] & 0b00000011) >> 0;
    }


    for(int j=0;j<0x200;j++)
    {
        osd1Lo[0][j] = hexLo[j+0xA00*2]; // start 2 sector
        osd1Lo[1][j] = hexLo[j]; // start 0 sector
        osd1Lo[2][j] = hexLo[j+0xA00*2+0x200];
        osd1Lo[3][j] = hexLo[j+0x200];
        osd1Lo[4][j] = hexLo[j+0xA00*2+0x400];
        osd1Lo[5][j] = hexLo[j+0x400];
        osd1Lo[6][j] = hexLo[j+0xA00*2+0x600];
        osd1Lo[7][j] = hexLo[j+0x600];
        osd1Lo[8][j] = hexLo[j+0xA00*2+0x800];
        osd1Lo[9][j] = hexLo[j+0x800];
    }


    for(int j=0;j<0x200;j++)
    {
        osd1Hi[0][j] = (hexLo[j+0xA00*1] & 0b00110000) >> 4; // start 1 sector
        osd1Hi[1][j] = (hexLo[j+0xA00*1] & 0b00000011) >> 0;
        osd1Hi[2][j] = (hexLo[j+0xA00*1+0x200] & 0b00110000) >> 4;
        osd1Hi[3][j] = (hexLo[j+0xA00*1+0x200] & 0b00000011) >> 0;
        osd1Hi[4][j] = (hexLo[j+0xA00*1+0x400] & 0b00110000) >> 4;
        osd1Hi[5][j] = (hexLo[j+0xA00*1+0x400] & 0b00000011) >> 0;
        osd1Hi[6][j] = (hexLo[j+0xA00*1+0x600] & 0b00110000) >> 4;
        osd1Hi[7][j] = (hexLo[j+0xA00*1+0x600] & 0b00000011) >> 0;
        osd1Hi[8][j] = (hexLo[j+0xA00*1+0x800] & 0b00110000) >> 4;
        osd1Hi[9][j] = (hexLo[j+0xA00*1+0x800] & 0b00000011) >> 0;
    }
//------------------------------------------------------------

    QString outputFileName = filename;

    outputFileName = outputFileName.mid(0,outputFileName.length()-4);

    QFile f0(outputFileName+".os0");
    QFile f1(outputFileName+".os1");

    f0.open(QIODevice::WriteOnly);
    f1.open(QIODevice::WriteOnly);

    if(!f0.isOpen() || !f1.isOpen())
    {
        return false;
    }


    f0.write("#ST92195\r\n");
    f1.write("#ST92195\r\n");
    for(int j =0;j<512;j++)
    {
        for(int i =0;i<10;i++)
        {

            QString q;

            if(osd0Hi[i][j] < 16) f0.write("0");
            q.setNum(int(osd0Hi[i][j]),16);q= q.toUpper();f0.write(q.toAscii());
            if(osd0Lo[i][j] < 16) f0.write("0");
            q.setNum(int(osd0Lo[i][j]),16);q= q.toUpper();f0.write(q.toAscii());
            f0.write(" ");


            if(osd1Hi[i][j] < 16) f1.write("0");
            q.setNum(int(osd1Hi[i][j]),16); q=q.toUpper();f1.write(q.toAscii());
            if(osd1Lo[i][j] < 16) f1.write("0");
            q.setNum(int(osd1Lo[i][j]),16);q= q.toUpper();f1.write(q.toAscii());
            f1.write(" ");

        }
        f0.write("\r\n");
        f1.write("\r\n");
    }

    f0.write("\r\n");
    f1.write("\r\n");

    f0.close();
    f1.close();

//    cout <<"OK"<< endl << "Complete!"<< endl;
    return true;
}

