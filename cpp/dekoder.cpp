#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdint>
using namespace std;

class Decoder {
protected:
    ofstream output;
public:
    Decoder(const string& filename) : output(filename, ios::out | ios::trunc) {} //trunc czysci plik output

    ofstream& getOutput() { return output; }

    uint64_t read5Bytes(const char* data)
    {
        return ((uint64_t)(unsigned char)data[0] << 32) |
               ((uint64_t)(unsigned char)data[1] << 24) |
               ((uint64_t)(unsigned char)data[2] << 16) |
               ((uint64_t)(unsigned char)data[3] << 8) |
               (uint64_t)(unsigned char)data[4];
    }

    uint32_t read4Bytes(const char* data)
    {
        return ((unsigned char)data[0] << 24) |
               ((unsigned char)data[1] << 16) |
               ((unsigned char)data[2] << 8) |
               (unsigned char)data[3];
    }

    long read3Bytes(const char* data)
    {
        return ((unsigned char)data[0] << 16) |
               ((unsigned char)data[1] << 8) |
               (unsigned char)data[2];
    }

    long read2Bytes(const char* data)
    {
        return ((unsigned char)data[0] << 8) |
               (unsigned char)data[1];
    }

    /*long read1Byte(const char* data) {
        return (unsigned char)data[0];
    }*/
    uint32_t read1Byte(const char* data) {
        return (unsigned char)data[0];
    }


    float read2BytesFloat(const char* data, float scale = 1.0f) {
        uint16_t raw = read2Bytes(data);
        return static_cast<float>(raw) / scale;
    }

    float read4BytesFloat(const char* data) {
        uint32_t val = 0;
        for (int i = 0; i < 4; ++i) {
            val = (val << 8) | static_cast<unsigned char>(data[i]);
        }
        float result;
        memcpy(&result, &val, sizeof(float));
        return result;
    }
};

class DecoderSec1 {
    Decoder& decoder;
public:
    DecoderSec1(Decoder& dec) : decoder(dec) {}

    long decode(const char* message) {
        ofstream& output = decoder.getOutput();
        output <<endl<< "===============SEKCJA 1===============" <<endl<<endl;;

        long iNBmsg1 = decoder.read3Bytes(message);
        output << "Dlugosc sekcji 1 (PDS - B1-3): " << iNBmsg1 << endl;

        char presence = message[7];
        int gds_flag = (presence & 128) >> 7;
        int bms_flag = (presence & 64) >> 6;
        output << "Obecnosc GDS (B8 Bit 1): " << gds_flag << endl;
        output << "Obecnosc BMS (B8 Bit 2): " << bms_flag << endl;

        output<< "Typ poziomu (B-10): " << decoder.read1Byte(&message[9])<< endl;
        output<< "pressure (B11-12): " << decoder.read2Bytes(&message[10])<<endl;
        output << "Data utworzenia komunikatu (B13-17): "
               << decoder.read1Byte(&message[12]) << "-"
               << decoder.read1Byte(&message[13]) << "-"
               << decoder.read1Byte(&message[14]) << " "
               << decoder.read1Byte(&message[15]) << ":"
               << decoder.read1Byte(&message[16]) << endl;

        return iNBmsg1;
    }
};

class DecoderSec2 {
    Decoder& decoder;
public:
    DecoderSec2(Decoder& dec) : decoder(dec) {}

    long decode(const char* message, long offset) {

        ofstream& output = decoder.getOutput();
        output <<endl<< "===============SEKCJA 2===============" <<endl<<endl;;


        const char* section2 = &message[offset];
        long iNBmsg2 = decoder.read3Bytes(section2);
        output << "Dlugosc sekcji 2 (GDS - B1-3): " << iNBmsg2 << endl;
        output << "Liczba wierszy: " << decoder.read2Bytes(&section2[8]) << endl;
        output << "Szerokosc poczatkowa w ms(B11-13): " << decoder.read3Bytes(&section2[10]) << endl;
        output << "Dlugosc poczatkowa w ms(B14-16): " << decoder.read3Bytes(&section2[13]) << endl;
        output << "Szerokosc koncowa w ms(B18-20): " << decoder.read3Bytes(&section2[17]) << endl;
        output << "Dlugosc koncowa w ms(B21-23): " << decoder.read3Bytes(&section2[20]) << endl;
        output << "(B29-33): "<< decoder.read5Bytes(&section2[28]) << endl;


        float value = decoder.read2BytesFloat(&section2[25], 1000.0f);
        output << "Incr N-S (float) (B26-27): " << value << endl;
        for (int i = 32; i < 178; i += 2) {
            int t = decoder.read2Bytes(&section2[i]);
            output << "Bajty " << i+1 << "-" << i+2 << ": " << t << endl;
        }


        return iNBmsg2;
    }
};

class DecoderSec4 {
    Decoder& decoder;
public:
    DecoderSec4(Decoder& dec) : decoder(dec) {}


   long decode(const char* message, long offset) {
    ofstream& output = decoder.getOutput();
    output << endl << "===============SEKCJA 4===============" << endl << endl;

    const char* section4 = &message[offset];
    long iNBmsg4 = decoder.read3Bytes(section4);
    output << "dlugosc sekcji 4 (B1-3): " << iNBmsg4 << endl;

    output << "wspolczynnik skali (B5-6): " << decoder.read2Bytes(&section4[4]) << endl;

    int A = section4[6] & 127;
    int B = decoder.read3Bytes(&section4[7]);

    output << "R: B(7-10):" << decoder.read4BytesFloat(&section4[6]) << endl;

    double R = (-1) * pow(-1, 5) * pow(2, -24) * B * pow(16, (A - 64));
    output << "wartosc referencyjna: " << R << endl;

    int bit_count = decoder.read1Byte(&section4[10]);
    output << "(B11): " << bit_count << endl;

    output << "5 kolejnych wartosci od bajtu 12: "<< decoder.read1Byte(&section4[11]) << ", ";
        output << decoder.read1Byte(&section4[12]) << ", ";
        output << decoder.read1Byte(&section4[13]) << ", ";
        output << decoder.read1Byte(&section4[14]) << ", ";
        output << decoder.read1Byte(&section4[15]) << endl;

    return iNBmsg4;
}

};

class DecoderSec5 {
    Decoder& decoder;
public:
    DecoderSec5(Decoder& dec) : decoder(dec) {}


    void decode(const char* message, long offset) {

        ofstream& output = decoder.getOutput();
        output <<endl<< "===============SEKCJA 5===============" <<endl<<endl;;


        const char* section5 = &message[offset];

        output << "B 1-4: ";
        for (int i = 0; i < 4; ++i) {
            output << section5[i];
        }
        output << endl;
    }
};

bool grib(const string& filename) {
    ifstream file(filename, ios::binary);
    Decoder decoder("output.txt");

    if (!file) {
        cerr << "blad pliku " << filename << endl;
        return false;
    }

    char buffer[4];
    int messageCount = 0;

    while (file.read(buffer, 4)) {
        if (buffer[0] == 'G' && buffer[1] == 'R' && buffer[2] == 'I' && buffer[3] == 'B') {
            ++messageCount;
            ofstream& output = decoder.getOutput();
            output << "\n================ GRIB MESSAGE #" << messageCount << " ================\n";

            char NBmsg[3];
            file.read(NBmsg, 3);
            long iNBmsg = decoder.read3Bytes(NBmsg);
            output << "calkowita dlugosc message (B5-7): " << iNBmsg << endl;

            char flag;
            file.read(&flag, 1);
            output << "flaga (B8): " << static_cast<int>((unsigned char)flag) << endl;

            char* message = new char[iNBmsg];
            memcpy(message, "GRIB", 4); // dopełniamy początek wiadomości
            memcpy(message + 4, NBmsg, 3);
            message[7] = flag;
            file.read(message + 8, iNBmsg - 8);

            DecoderSec1 sec1(decoder);
            long sec1_len = sec1.decode(message + 8);

            DecoderSec2 sec2(decoder);
            long sec2_len = sec2.decode(message + 8, sec1_len);

            DecoderSec4 sec4(decoder);
            long sec4_len = sec4.decode(message + 8, sec1_len + sec2_len);

            DecoderSec5 sec5(decoder);
            sec5.decode(message + 8, sec1_len + sec2_len + sec4_len);

            delete[] message;

            //szuka griba 2
            file.seekg(-(iNBmsg - 8), ios::cur);
        }
        else {
            //przesuwa
            file.seekg(-3, ios::cur);
        }
    }

    if (messageCount == 0) {
        decoder.getOutput() << "nie ma GRIB" << endl;
        return false;
    }

    return true;
}


int main() {
    string filename = "20150310.00.W.dwa_griby.grib";
    grib(filename);
    return 0;
}
