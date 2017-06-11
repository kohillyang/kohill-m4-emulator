#include "public_header.hpp"
class Intel_HEX_Adapter : public QObject{
    class HEX_LINE{
    public:
        uint8_t startcode;
        int byte_count;
        uint16_t address;
        uint8_t record_type;
        QByteArray data;
        uint8_t checksum;
        QString toString(){
        	return QString("%1 byte_count:%2 address:0x%L3 record_type:%4 data:0x%5 checksum:%6")
        			.arg((char)startcode)
        			.arg(byte_count)
					.arg(address,0,16)
					.arg(record_type)
					.arg(QString(data),0,16)
					.arg(checksum);
        }
    } ;
public:
    QList<QByteArray> getlines(){
        QList<QByteArray> r;
        QFile file(this->filename);  
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  
        	qDebug()<<"Can't open the file!"<<endl;
        	return r;
        }  
        while(!file.atEnd()) {  
            QByteArray line = file.readLine();  
            r.push_back(line);
            parseLine(line);
        }  
        file.close();
        return r;
    
    }

    HEX_LINE parseLine(QByteArray line){
    	HEX_LINE r;
    	r.startcode = line[0];
    	bool ok;
    	r.byte_count = line.mid(1, 2).toInt(&ok,16);
    	r.address = (line.mid(3, 4)).toInt(&ok, 16);
    	r.record_type = line.mid(7, 2).toInt(&ok,16);
    	r.data = line.mid(9,r.byte_count*2);
    	r.checksum = line.mid(9 + r.byte_count * 2, 2).toInt(&ok, 16);
    	return r;
    }
    void printLines(){
    	QList<QByteArray> lines = getlines();
    	for(auto iter = lines.begin();iter!= lines.end();iter ++){
        	cout << QString(*iter);
    		cout << parseLine(*iter).toString() << endl;
    	}
    }
    Intel_HEX_Adapter(QString filename){
        this->filename = filename;
    }
    ~Intel_HEX_Adapter(){
    }
    QMap<uint32_t,uint32_t> toBinary(){
    	QMap<uint32_t,uint32_t> r;
    	QList<QByteArray> lines = getlines();
    	uint32_t baseAddr = 0;
    	bool ok = false;
		const int size_t_width = 8;

    	for(auto iter = lines.begin();iter!= lines.end();iter ++){
    		auto pLine =  parseLine(*iter);
    		switch (pLine.record_type) {
    		case 0x00:
					for(int i = 0;i <  pLine.byte_count /size_t_width ; i++){
						uint32_t dat = 0;
						QByteArray da_32_str = pLine.data.mid(i*size_t_width, size_t_width);
						for (int var = 0; var < 4; ++var) {
							auto da_byte_str = da_32_str.mid(var*2,2);
							uint32_t dat_8 = da_byte_str.toUInt(&ok, 16);
							dat |= dat_8 << (8*var);
						}
						r[baseAddr + pLine.address + size_t_width/2 * i] =dat;
					}//Data
					break;
				case 0x01:
				//End Of File
					return r;
					break;
				case 0x02:break;
				//Extended Segment Address,No need to implement.
				case 0x03:break;
				//Start Segment Address, No need to implement.
				case 0x04:
					baseAddr = (pLine.data).toUInt(&ok, 16) * 65536;
					break;
				//Extended Linear Address
				case 0x05:break;
				//Start Linear Address, this shows the entry address.
				default:
					break;
			}
    	}
    	return r;
    }
private:
    QString filename;
};


