#include <iostream>
using namespace std;
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#ifndef NO_MFMXML
#include <tinyxml.h>
#endif
#include "MFMAllFrames.h"
#include "DataParameters.h"
#include "DataScalers.h"
#include "MError.h"
#include "Cobo.h"
#include <cstring>
#include <cstdlib>
#include <time.h>
#include "MFMlib.in.h"

MFMCommonFrame * fFrame;
MFMCoboFrame * fCoboframe;
MFMCoboTopoFrame * fCobotopoframe;
MFMExogamFrame * fExoframe;
MFMDiamantFrame * fDiamantframe;
MFMEbyedatFrame * fEbyframe;
MFMCommonFrame* fInsideframe;
MFMMergeFrame * fMergeframe;
MFMOscilloFrame * fOscilloframe;
MFMScalerDataFrame * fScalerframe;
MFMRibfFrame * fRibfframe;
MFMMutantFrame * fMutframe;
MFMHelloFrame * fHelloframe;
MFMXmlDataDescriptionFrame * fDatadescriptionframe;
MFMXmlFileHeaderFrame * fHeaderframe;
MFMChimeraFrame *fChimeraframe;
MFMBoxDiagFrame * fBoxDiagframe;
MFMVamosICFrame * fVamosICframe;
MFMVamosPDFrame * fVamosPDframe;
MFMVamosTACFrame * fVamosTACframe;
MFMNedaFrame * fNedaframe;
MFMNedaCompFrame * fNedaCompframe;
MFMS3BaF2Frame * fS3BaF2frame;
MFMS3AlphaFrame * fS3Alphaframe;
MFMS3RuthFrame * fS3Ruthframe;
MFMS3eGUNFrame * fS3eGUNframe;
MFMReaGenericFrame * fReaGenericframe;
MFMReaScopeFrame * fReaScopeframe;
MFMS3SynchroFrame * fS3Synchroframe;
DataParameters * fDataPara;
DataScalers * fDataScal;

bool fInitebydone;
MError Error;
int fDumpsize;
int fSubDumpsize;
int fMaxdump;
int fCount_elseframe;
int fNbFrames; // nb of frame  to generate
int fNbFramesRead; // nb of frame  to read
int fNbSubFrames;
int fNbFramesStart;// no of frame to start the dump if dump is asked
int fNbFramesDump; // no of frame to the dump if dump is asked
uint32_t fEventNumber;

int fVerbose;
int fFormat;
int fNbOfEvent;
const char * fDeffilename = "mfmfile.dat";
long long fPtInFile =0;

#define ADONIS_MSK 0x3FFF

bool InfoCondition();
void Announce();
void ReadUserFrame(MFMCommonFrame* commonframe);
void ReadMergeFrame(MFMCommonFrame* commonframe);
void ReadCoboFrame(MFMCommonFrame* commonframe);
void ReadExogamFrame(MFMCommonFrame* commonframe);
void ReadEbyedatFrame(MFMCommonFrame* commonframe);
void ReadCoboTopoFrame(MFMCommonFrame* commonframe);
void ReadScalerFrame(MFMCommonFrame* commonframe);
void ReadRibfFrame(MFMCommonFrame* commonframe);
void ReadOscilloFrame(MFMCommonFrame* commonframe);
void ReadMutantFrame(MFMCommonFrame* commonframe);
void ReadXmlDataDescriptionFrame(MFMCommonFrame* commonframe);
void ReadXmlHeaderFrame(MFMCommonFrame* commonframe);
void ReadHelloFrame(MFMCommonFrame* commonframe);
void ReadDiamantFrame(MFMCommonFrame* commonframe);
void ReadDefaultFrame(MFMCommonFrame* commonframe);
void ReadBoxDiagFrame(MFMCommonFrame* commonframe);
void ReadChimeraFrame(MFMCommonFrame* commonframe);
void ReadVamosICFrame(MFMCommonFrame* commonframe);
void ReadVamosPDFrame(MFMCommonFrame* commonframe);
void ReadVamosTACFrame(MFMCommonFrame* commonframe);
void ReadNedaFrame(MFMCommonFrame* commonframe);
void ReadNedaCompFrame(MFMCommonFrame* commonframe);
void ReadS3BaF2Frame(MFMCommonFrame* commonframe);
void ReadS3AlphaFrame(MFMCommonFrame* commonframe);
void ReadS3RuthFrame(MFMCommonFrame* commonframe);
void ReadS3eGUNFrame(MFMCommonFrame* commonframe);
void ReadS3SunchroFrame(MFMCommonFrame* commonframe);
void ReadReaGenericFrame(MFMCommonFrame* commonframe);
void ReadReaScopeFrame(MFMCommonFrame* commonframe);


void WriteUserFrame(int flun, int format, int fNbFrames, int fNbSubFrames = 5);
void WriteMergeFrame(int flun, int fNbFrames, int type, int fNbSubFrames = 5);
void WriteCoboFrame(int flun, int fNbFrames, bool full = false);
void WriteXmlDataDescriptionFrame(int lun, int fNbFrames);
void WriteExogamFrame(int flun, int fNbFrames);
void WriteNedaBisFrame(int lun, int fNbFrames);
void WriteDiamantFrame(int lun, int fNbFrames);
void WriteScalerFrame(int lun, int fNbFrames);
void WriteMutantFrame(int lun, int fNbFrames);
void WriteHelloFrame(int lun, int fNbFrames);
void WriteOscilloFrame(int lun, int fNbFrames);
void WriteEbyedatFrame(int lun, int fNbFrames, int type);
void WriteRibfFrame(int lun, int fNbFrames);
void WriteBoxDiagFrame(int lun, int fNbFrames);
void WriteVamosICFrame(int lun, int fNbFrames);
void WriteVamosPDFrame(int lun, int fNbFrames);
void WriteVamosTACFrame(int lun, int fNbFrames);
void WriteNedaFrame(int lun, int fNbFrames);
void WriteNedaCompFrame(int lun, int fNbFrames);
void WriteS3BaF2Frame(int lun, int fNbFrames);
void WriteS3AlphaFrame(int lun, int fNbFrames);
void WriteS3RuthFrame(int lun, int fNbFrames);
void WriteS3eGUNFrame(int lun, int fNbFrames);
void WriteS3SynchroFrame(int lun, int fNbFrames);
void WriteReaGenericFrame(int lun, int fNbFrames);
void WriteReaScopeFrame(int lun, int fNbFrames);

void PrintQuestion();
void ConversionAdonisToFrames(int lun);
void Statistics(bool writeorread = false);
void Help();
void ErrorMessage();

int main(int argc, char **argv) {
    MError Error;
    char* tempo[128];
    char *tempos = NULL, *wordtempo=NULL;
    bool writeorread = false;
    int treatedinfo = 0;
    bool all = false;
    char filename[255];

    fDumpsize = 16;
    fSubDumpsize = 16;
    fCount_elseframe = 0;
    fMaxdump = 128;
    fFormat = 0;
    fNbFrames = 20;
    fNbFramesRead = -1;
    fNbSubFrames = 5;
    fNbFramesStart = 0;
    fNbFramesDump = 0;
    fVerbose = 5;

    strcpy(filename, fDeffilename);

    fDataPara = new DataParameters();
    fDataScal = new DataScalers();

    fFrame = new MFMCommonFrame();
    fCoboframe = new MFMCoboFrame();
    fCobotopoframe = new MFMCoboTopoFrame();
    fExoframe = new MFMExogamFrame();
    fDiamantframe = new MFMDiamantFrame();
    fEbyframe = new MFMEbyedatFrame();
    fInsideframe = new MFMCommonFrame();
    fMergeframe = new MFMMergeFrame();
    fOscilloframe = new MFMOscilloFrame();
    fScalerframe = new MFMScalerDataFrame();
    fRibfframe = new MFMRibfFrame();
    fMutframe = new MFMMutantFrame();
    fHelloframe = new MFMHelloFrame();
    fDatadescriptionframe = new MFMXmlDataDescriptionFrame();
    fHeaderframe = new MFMXmlFileHeaderFrame();
    fChimeraframe = new MFMChimeraFrame();
    fBoxDiagframe = new MFMBoxDiagFrame();
    fVamosICframe = new MFMVamosICFrame();
    fVamosPDframe = new MFMVamosPDFrame();
    fVamosTACframe = new MFMVamosTACFrame();
    fNedaframe = new MFMNedaFrame();
    fNedaCompframe = new MFMNedaCompFrame();
    fS3BaF2frame = new MFMS3BaF2Frame();
    fS3Alphaframe = new MFMS3AlphaFrame();
    fS3Ruthframe = new MFMS3RuthFrame();
    fS3eGUNframe = new MFMS3eGUNFrame();
    fS3Synchroframe = new MFMS3SynchroFrame();
    fReaGenericframe = new MFMReaGenericFrame();
    fReaScopeframe = new MFMReaScopeFrame();

    fScalerframe->InitStat();
    fCoboframe->InitStat();
    fExoframe->InitStat();
    fEbyframe->InitStat();
    fMergeframe->InitStat();
    fScalerframe->InitStat();
    fOscilloframe->InitStat();
    fInsideframe->InitStat();
    fRibfframe->InitStat();
    fMutframe->InitStat();
    fDatadescriptionframe->InitStat();
    fHeaderframe->InitStat();
    fHelloframe->InitStat();
    fChimeraframe ->InitStat();
    fBoxDiagframe ->InitStat();
    fVamosICframe ->InitStat();
    fVamosPDframe ->InitStat();
    fVamosTACframe ->InitStat();
    fDiamantframe->InitStat();
    fNedaframe->InitStat();
    fNedaCompframe ->InitStat();
    fS3BaF2frame->InitStat();
    fS3Alphaframe->InitStat();
    fS3Ruthframe->InitStat();
    fS3eGUNframe->InitStat();
    fS3Synchroframe->InitStat();
    fReaGenericframe->InitStat();
    fReaScopeframe->InitStat();
    for (int i = 1; i < argc; i++) {

        treatedinfo = 0;

        if ((strcasecmp(argv[i], "-h") == 0) or (strcasecmp(argv[i], "--help")
                                                 == 0)) {
            Help();
            return (0);
        }

        if ((strcasecmp(argv[i], "-ver") == 0) or (strcasecmp(argv[i], "--version")
                                                   == 0)) {
            Announce();
            return (0);
        }

        tempos = new char[strlen(argv[i]) + 1];
        tempos = strcpy(tempos, argv[i]);
        strcpy(tempos, argv[i]);
        wordtempo = strtok(tempos, "=");

        if ((strcasecmp(wordtempo, "-n") == 0) or (strcasecmp(wordtempo,
                                                              "--number") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fNbFrames = atoi(wordtempo);
            treatedinfo++;
        }
        if ((strcasecmp(wordtempo, "-nr") == 0) or (strcasecmp(wordtempo,
                                                               "--numberread") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fNbFramesRead = atoi(wordtempo);
            treatedinfo++;
        }
        if ((strcasecmp(wordtempo, "-nt") == 0) or (strcasecmp(wordtempo,
                                                               "--numberstart") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fNbFramesStart = atoi(wordtempo);
            treatedinfo++;
        }
        if ((strcasecmp(wordtempo, "-nd") == 0) or (strcasecmp(wordtempo,
                                                               "--numberdump") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fNbFramesDump = atoi(wordtempo);
            treatedinfo++;
        }


        if ((strcasecmp(wordtempo, "-sn") == 0) or (strcasecmp(wordtempo,
                                                               "--subnumber") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fNbSubFrames = atoi(wordtempo);
            treatedinfo++;
        }

        if ((strcasecmp(wordtempo, "-f") == 0) or (strcasecmp(wordtempo,
                                                              "--file") == 0)) {
            wordtempo = strtok(NULL, "=");

            if (wordtempo == NULL)
                ErrorMessage();
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            strcpy(filename, wordtempo);
            treatedinfo++;
        }

        if ((strcasecmp(wordtempo, "-d") == 0) or (strcasecmp(wordtempo,
                                                              "--dump") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (wordtempo == NULL)
                ErrorMessage();
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fDumpsize = atoi(wordtempo);
            treatedinfo++;
        }
        if ((strcasecmp(wordtempo, "-sd") == 0) or (strcasecmp(wordtempo,
                                                               "--subdump") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (wordtempo == NULL)
                ErrorMessage();
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fSubDumpsize = atoi(wordtempo);
            treatedinfo++;
        }

        if ((strcasecmp(wordtempo, "-a") == 0) or (strcasecmp(wordtempo,
                                                              "--all") == 0)) {
            all = true;
            treatedinfo++;
        }
        if ((strcasecmp(wordtempo, "-w") == 0) or (strcasecmp(wordtempo,
                                                              "--write") == 0)) {
            writeorread = true;
            treatedinfo++;
        }

        if ((strcasecmp(wordtempo, "-r") == 0) or (strcasecmp(wordtempo,
                                                              "--read") == 0)) {
            writeorread = false;
            treatedinfo++;
        }
        if ((strcasecmp(wordtempo, "-v") == 0) or (strcasecmp(wordtempo,
                                                              "--verbose") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (wordtempo == NULL)
                ErrorMessage();
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fVerbose = atoi(wordtempo);
            treatedinfo++;
        }
        if ((strcasecmp(wordtempo, "-fo") == 0) or (strcasecmp(wordtempo,
                                                               "--format") == 0)) {
            wordtempo = strtok(NULL, "=");
            if (wordtempo == NULL)
                ErrorMessage();
            if (strcasecmp(wordtempo, "") == 0)
                ErrorMessage();
            fFormat = atoi(wordtempo);
            treatedinfo++;
        }
        if (treatedinfo == 0) {
            ErrorMessage();

        }
        if (tempos) delete [] tempos;
    }
    Announce();
    if ((argc > 1)and (treatedinfo>0)) {
        //_____________________ WRITE ____________________________________________________________
        if (writeorread) {

            cout << endl;
            cout << "-------------------------------------------------" << endl;
            cout << "|    WRITE of " << fNbFrames << " MFMFrame in file  "
				<< filename << "" << endl;
            cout << "-------------------------------------------------" << endl;
            cout << endl;

            fEventNumber = 0;

            int fLun; // Logical Unit Number
            fLun = open(filename, (O_RDWR | O_CREAT | O_TRUNC), 0644);

            if (fFormat == 0) {
                while ((fFormat < 1) or (fFormat > 200)) {
                    PrintQuestion();
                    cin >> fFormat;
                }

            }
            WriteUserFrame(fLun, fFormat, fNbFrames, fNbSubFrames);
            close(fLun);

        }

        //_____________________ READ____________________________________________________________

        if (writeorread == false) {
            cout << "|   READ MFMFrame from file " << filename << "" << endl;
            cout << "-------------------------------------------------------------"
				<< endl;

            cout << endl;

            fInitebydone = false;
            fMaxdump = 128;
            int fLun; // Logical Unit Number
            //------------------------------------------------------------------------
            // Test xml a enlever des que c'est prêt
            //

            //DataParameters datapar;
            // datapar.FillFromActionXMLFile((const char*)filename);

            // datapar.DumpListOfNames();
            //datapar.CreatXML();
            //datapar.WriteXML("bidon1.xml");

            //------------------------------------------------------------------------
            //------------------------------------------------------------------------
            // Test xml a enlever des que c'est prêt
            /*
        cout <<"-----------------0-----------------------------------\n";
         DataScalers datascal;
         cout <<"-----------------1-----------------------------------\n";
         datascal.FillFromActionXMLFile((const char*)filename);
         cout <<"-----------------2-----------------------------------\n";
         datascal.DumpListOfNames();
         cout <<"-----------------3-----------------------------------\n";
         datascal.CreatXML();
         cout <<"-----------------4-----------------------------------\n";
         datascal.WriteXML("bidon.xml");
         cout <<"-----------------5-----------------------------------\n";
         return 0;
         */
            //------------------------------------------------------------------------

            fLun = open(filename, (O_RDONLY));
            if (fLun <= 0) {
                Error.TreatError(2, 0, "Error open file :", filename);

            }

            int minsizeheader = MFM_BLOB_HEADER_SIZE; // =8
            int cont = 0;
            fNbOfEvent = 0;

            char* vector;
            char ** pvector;
            int vectorsize = minsizeheader;
            vector = (char*) (malloc(vectorsize));
            pvector = &vector;
            int framesize = 0;

            while (fNbOfEvent != fNbFramesRead) {
                framesize = fFrame->ReadInFile(&fLun, pvector, &vectorsize);
                fPtInFile+=framesize;
                if (framesize <= 0)
                    break;
                if ((fVerbose > 1) && InfoCondition()) {
                    cout << "\n-- Read frame in file : nb = "
                        << dec << fNbOfEvent
                         <<"----------------------------------\n";
                }

                        ReadUserFrame(fFrame);
                fNbOfEvent++;
                fFrame->SetAttributs();

            }
            if(*pvector) free (*pvector);
        }
        Statistics(writeorread);
    }else{
        Help();
    }

    if (fDataPara) delete(fDataPara) ;
    if (fDataScal) delete (fDataScal);
    if (fFrame)
        delete (fFrame);
    if (fCoboframe)
        delete (fCoboframe);
    if (fCobotopoframe)
        delete (fCobotopoframe);
    if (fExoframe)
        delete (fExoframe);
    if (fEbyframe)
        delete (fEbyframe);
    if (fDiamantframe)
        delete (fDiamantframe);
    if (fInsideframe)
        delete (fInsideframe);
    if (fMergeframe)
        delete (fMergeframe);
    if (fOscilloframe)
        delete (fOscilloframe);
    if (fScalerframe)
        delete (fScalerframe);
    if (fRibfframe)
        delete (fRibfframe);
    if (fMutframe)
        delete (fMutframe);
    if (fHelloframe)
        delete (fHelloframe);
    if (fDatadescriptionframe)
        delete (fDatadescriptionframe);
    if (fHeaderframe)
        delete (fHeaderframe);
    if (fChimeraframe)
        delete (fChimeraframe);
    if (fBoxDiagframe)
        delete (fBoxDiagframe);
    if (fVamosPDframe)
        delete (fVamosPDframe);
    if (fVamosICframe)
        delete (fVamosICframe);
    if (fVamosTACframe)
        delete (fVamosTACframe);
    if (fNedaframe)
        delete (fNedaframe);
    if (fNedaCompframe)
        delete (fNedaCompframe);
    if (fS3BaF2frame)
        delete (fS3BaF2frame);
    if (fS3Alphaframe)
        delete (fS3Alphaframe);
    if (fS3Ruthframe)
        delete (fS3Ruthframe);
    if (fS3eGUNframe)
        delete (fS3eGUNframe);
    if (fS3Synchroframe)
        delete (fS3Synchroframe);
    if (fReaGenericframe) delete (fReaGenericframe);
    if (fReaScopeframe ) delete (fReaScopeframe);

    cout << "-------------------End------------------------" << endl;
    return (0);
}
//_______________________________________________________________________________________________________________________
void PrintQuestion() {
    cout << "Which Frame you want to generate?\n";
    cout << "   1  Cobo\n";
    cout << "   2  Cobo Full\n";
    cout << "   3  Exogam Energy\n";
    cout << "   4  Exogam Oscilloscope\n";
    cout << "   5  NEDA Raw \n";
    cout << "   6  NEDA Compressed  \n";
    cout << "   7  Ebyedat EventNumber\n";
    cout << "   8  Ebyedat TimeStamp\n";
    cout << "   9  Ebyedat EventNumberTimeStamp\n";
    cout << "  10  Scaler data Frame\n";
    cout << "  11  Mutant data Frame\n";
    cout << "  12  RIBF data Frame\n";
    cout << "  13  Hello Frame\n";
    cout << "  14  Box Diagnostic Frame\n";
    cout << "  15  Vamos Ionization Chamber Frame\n";
    cout << "  16  Vamos Position Detector Frame\n";
    cout << "  17  Vamos TAC Frame\n";
    cout << "  18  Diamant Frame\n";
    cout << "  19  S3 BaF2 Frame\n";
    cout << "  20  S3 Si Alpha Frame\n";
    cout << "  21  S3 Rutherford Frame\n";
    cout << "  22  S3 eGUN Frame\n";
    cout << "  23  S3 Synchro Frame\n";
    cout << "  24  S3 ReaGeneric Frame\n";
    cout << "  25  S3 ReaScope Frame\n";

    cout << "  26  Xml Data Description Frame\n";
    cout << "  99  Mixte of all Frame\n";

    cout << "\n";
    cout << " 100  Merge of Ebyedats in EventNumber\n";
    cout << " 101  Merge of Ebyedats in TimeStamp\n";
    cout << " 102  Merge of Cobo in EventNumber\n";
    cout << "\n";
    cout << " 200  Conversion Adonis in Ebyedat\n";
    cout << " >>";
}
//_______________________________________________________________________________________________________________________
void Announce(){
    cout << "-------------------------------------------------" << endl;
    cout << "---- MFMtest.exe Version : "<<BUILD_MFM_DATE<<" "<<BUILD_MFM_TIME<<" ----" << endl;
    cout << "-------------------------------------------------" << endl;
}
//_______________________________________________________________________________________________________________________
void Help() {
    Announce();
    cout
            << "\n MFMtest.exe : utility to generate and use MFM frame in different formats \n";
    cout << " The generated frames are stored in a file  \n";
    cout << " Usage  : \n";
    cout << "          MFMtest.exe [-h -r -w -f -n -fo -sn -d -sd -ver -v]      \n";
    cout << "            -h or --help , print this help \n";
    cout << "            -ver or --version , print version \n";
    cout
            << "            -f=filname or --file=filename , set name of file to read or to write , default is \""
			<< fDeffilename << "\"\n";
    cout << "            -w or --write, set write mode \n";
    cout << "            -r or --read, set read mode \n";
    cout
            << "            -n=nb or --number=nb,  set nb number of frames to write, default is"
			<< fNbFrames << " \n";
    cout
            << "            -nr=nb or --numberread=nb,  set nb number of frames to read, default is"
			<< fNbFramesRead << " \n";
    cout
            << "            -nd=nb or --numberdump=nb,  set nb number of frames to dump if asked, default is all \n";
    cout
            << "            -nt=nb or --numberstart=nb,  set nb  number of frames to start dump  if  asked (defautlt =  "
			<< fNbFrames << ") \n";
    cout
            << "            -sn=nb or --subnumber=nb, set nb number (nb) of subframes to write, default is "
			<< fNbFrames << " \n";
    cout
            << "            -v=nb or --verbose, give more information during read or write(default : "
			<< fVerbose << ") \n";
    cout
            << "            -d=nb or --dump=nb,  number of bytes to dump.(defautlt = "
			<< fSubDumpsize << ") \n";
    cout
            << "            -sd=nb or --subdump=nb,  number of bytes to dump in frame inside a merge frame.(defautlt = "
			<< fSubDumpsize << ") \n";
    cout
            << "            -fo=nb or --format=nb , set format of frame to write, if not precised, you will have this menu to chose\n";
    PrintQuestion();
    cout << endl;
    cout << endl;
    cout
            << "   Examples MFMtest.exe --write -fo=2 -n=100 : write with a choosen format = 2 and with 100 generated frames\n";
    cout << "            MFMtest.exe --read           : read file \""
			<< fDeffilename << "\" \n";
    cout
            << "            MFMtest.exe --read -f=run.dat: read \"run.dat\" file \n";
    cout << endl;
}
//_______________________________________________________________________________________________________________________

void ErrorMessage() {
    cout << " Command not good, see help with MFMtest.exe -h\n";
    exit(0);
}
//_______________________________________________________________________________________________________________________

void ReadUserFrame(MFMCommonFrame* commonframe) {

    commonframe->SetAttributs();
    int type = commonframe->GetFrameTypeAttribut();
    int headersize = commonframe->GetHeaderSizeAttribut();
    // part of use of frame
    // example

    switch (type) {

    case MFM_COBOF_FRAME_TYPE:
    case MFM_COBO_FRAME_TYPE: {
        ReadCoboFrame(commonframe);
        break;
    }
    case MFM_COBOT_FRAME_TYPE: {
        ReadCoboTopoFrame(commonframe);
        break;
    }
    case MFM_EXO2_FRAME_TYPE: {
        ReadExogamFrame(commonframe);
        break;
    }
    case MFM_EBY_EN_FRAME_TYPE:
    case MFM_EBY_TS_FRAME_TYPE:
    case MFM_EBY_EN_TS_FRAME_TYPE: {
        ReadEbyedatFrame(commonframe);
        break;
    }
    case MFM_SCALER_DATA_FRAME_TYPE: {
        ReadScalerFrame(commonframe);
        break;
    }
    case MFM_RIBF_DATA_FRAME_TYPE: {
        ReadRibfFrame(commonframe);
        break;
    }
    case MFM_MUTANT_FRAME_TYPE: {
        ReadMutantFrame(commonframe);
        break;
    }
    case MFM_HELLO_FRAME_TYPE: {
        ReadHelloFrame(commonframe);
        break;
    }
    case MFM_DIAMANT_FRAME_TYPE: {

        ReadDiamantFrame(commonframe);
        break;
    }
    case MFM_OSCI_FRAME_TYPE: {
        ReadOscilloFrame(commonframe);
        break;
    }
    case MFM_MERGE_EN_FRAME_TYPE:
    case MFM_MERGE_TS_FRAME_TYPE: {
        ReadMergeFrame(commonframe);
        break;
    }
    case MFM_XML_DATA_DESCRIPTION_FRAME_TYPE: {
        ReadXmlDataDescriptionFrame(commonframe);
        break;
    }
    case MFM_XML_FILE_HEADER_FRAME_TYPE: {
        ReadXmlHeaderFrame(commonframe);
        break;
    }
    case MFM_CHIMERA_DATA_FRAME_TYPE: {
        ReadChimeraFrame(commonframe);
        break;
    }
    case MFM_BOX_DIAG_FRAME_TYPE: {
        ReadBoxDiagFrame(commonframe);
        break;
    }
    case MFM_VAMOSIC_FRAME_TYPE: {
        ReadVamosICFrame(commonframe);
        break;
    }
    case MFM_VAMOSPD_FRAME_TYPE: {
        ReadVamosPDFrame(commonframe);
        break;
    }
    case MFM_VAMOSTAC_FRAME_TYPE: {
        ReadVamosTACFrame(commonframe);
        break;
    }
    case MFM_NEDA_FRAME_TYPE: {
        ReadNedaFrame(commonframe);
        break;
    }
    case MFM_NEDACOMP_FRAME_TYPE: {
        ReadNedaCompFrame(commonframe);
        break;
    }
    case MFM_S3_BAF2_FRAME_TYPE: {
        ReadS3BaF2Frame(commonframe);
        break;
    }
    case MFM_S3_ALPHA_FRAME_TYPE: {
        ReadS3AlphaFrame(commonframe);
        break;
    }
    case MFM_S3_RUTH_FRAME_TYPE: {
        ReadS3RuthFrame(commonframe);
        break;
    }
    case MFM_S3_EGUN_FRAME_TYPE: {
        ReadS3eGUNFrame(commonframe);
        break;
    }
    case MFM_S3_SYNC_FRAME_TYPE: {
        ReadS3BaF2Frame(commonframe);
        break;
    }
    case MFM_REA_SCOPE_FRAME_TYPE: {
        ReadReaScopeFrame(commonframe);
        break;
    }
    case MFM_REA_GENE_FRAME_TYPE: {
        ReadReaGenericFrame(commonframe);
        break;
    }

    default: {
        fCount_elseframe++;

        ReadDefaultFrame(commonframe);
        break;
    }

    }// end of switch
}
//_______________________________________________________________________________________________________________________

bool InfoCondition() {
    return (((fNbFramesDump == 0) && (fNbFramesStart <= fNbOfEvent))
            || ((fNbFramesStart <= fNbOfEvent) && (fNbFramesStart
                                                   + fNbFramesDump > fNbOfEvent)));
}
//_______________________________________________________________________________________________________________________
void ReadCoboFrame(MFMCommonFrame* commonframe) {
    fCoboframe->SetAttributs(commonframe->GetPointHeader());
    char info[64];
    int type = fCoboframe->GetFrameTypeAttribut();
    int framesize = commonframe->GetFrameSize();
    fCoboframe->FillStat();

    if (type == MFM_COBO_FRAME_TYPE)
        strcpy(info, "--Cobo Frame--");
    if (type == MFM_COBOF_FRAME_TYPE)
        strcpy(info, "--CoboFull Frame--");
    if ((fVerbose > 1) && InfoCondition()) {
        fCoboframe->HeaderDisplay(info);
        if (fVerbose > 3) {
            int dump = fDumpsize;
            if (framesize < dump)
                dump = framesize;
            fCoboframe->DumpRaw(dump, 0);
        }
    }
}

//_______________________________________________________________________________________________________________________
void ReadDefaultFrame(MFMCommonFrame* commonframe) {

    if ((fVerbose > 1) && InfoCondition()) {
        commonframe->HeaderDisplay();
        if (fVerbose > 3) {
            int framesize = commonframe->GetFrameSize();
            int dump = fDumpsize;
            if (framesize < dump)
                dump = framesize;
            commonframe->DumpRaw(dump, 0);
        }
    }
}

//_______________________________________________________________________________________________________________________
void ReadMergeFrame(MFMCommonFrame* commonframe) {

    fMergeframe->SetAttributs(commonframe->GetPointHeader());
    int nbinsideframe = 0;
    nbinsideframe = fMergeframe->GetNbItemsAttribut();
    fMergeframe->FillStat();
    int framesize = fMergeframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fMergeframe->HeaderDisplay();
        if (fVerbose > 3) {
            int dump = fDumpsize;
            if (framesize < dump)
                dump = framesize;
            fMergeframe->DumpRaw(dump, 0);
        }
    }
    fMergeframe->ResetReadInMem();
    for (int i = 0; i < nbinsideframe; i++) {
        fMergeframe->ReadInFrame(fInsideframe);
        fInsideframe->FillStat();
        ReadUserFrame(fInsideframe);
    }
}

//_______________________________________________________________________________________________________________________
void ReadCoboTopoFrame(MFMCommonFrame* commonframe) {
    fCobotopoframe->SetAttributs(commonframe->GetPointHeader());
    fCobotopoframe->FillStat();
    int framesize = fCobotopoframe->GetFrameSize();

    if ((fVerbose > 1) && InfoCondition()) {
        fCobotopoframe->HeaderDisplay((char*) "--CoboTopology Frame--");

        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fCobotopoframe->DumpRaw(dump, 0);
    }
}

//_______________________________________________________________________________________________________________________

void ReadEbyedatFrame(MFMCommonFrame* commonframe) {

    fEbyframe->SetAttributs(commonframe->GetPointHeader());
    fEbyframe->FillStat();
    int framesize = fEbyframe->GetFrameSize();

    if ((fVerbose > 1) && InfoCondition()) {
        fEbyframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fEbyframe->DumpRaw(dump, 0);

        if (!fInitebydone) {
            fInitebydone = true;
            //cout << "nb of parameters = :"<<datapar.FillFromActionFile("ACTIONS_test.CHC_PAR")<<"\n";
            //datapar.DumpListPara();
            //datapar.TestParameters();
        }
    }
    //cout << " Couples of label/value : \n"
    //		<< (fEbyframe->DumpData());
}
//_______________________________________________________________________________________________________________________
void ReadExogamFrame(MFMCommonFrame* commonframe) {

    fExoframe->SetAttributs(commonframe->GetPointHeader());
    int framesize = fExoframe->GetFrameSize();
    fExoframe->FillStat();
    if ((fVerbose > 1) && InfoCondition()) {
        fExoframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fExoframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadDiamantFrame(MFMCommonFrame* commonframe) {
    fDiamantframe->SetAttributs(commonframe->GetPointHeader());
    int framesize = fDiamantframe->GetFrameSize();
    fDiamantframe->FillStat();
    if ((fVerbose > 1) && InfoCondition()) {
        fDiamantframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fDiamantframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadScalerFrame(MFMCommonFrame* commonframe) {
    fScalerframe->SetAttributs(commonframe->GetPointHeader());
    fScalerframe->FillStat();
    int framesize = fScalerframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fScalerframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fScalerframe->DumpRaw(dump, 0);
        if (fVerbose > 5)
            cout << fScalerframe-> GetDumpTextData()
                 << (fScalerframe->GetDumpData());
    }
}
//_______________________________________________________________________________________________________________________
void ReadRibfFrame(MFMCommonFrame* commonframe) {
    fRibfframe->SetAttributs(commonframe->GetPointHeader());
    fRibfframe->FillStat();
    int framesize = fRibfframe->GetFrameSize();

    if ((fVerbose > 1) && InfoCondition()) {
        fRibfframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fRibfframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadMutantFrame(MFMCommonFrame* commonframe) {

    fMutframe->SetAttributs(commonframe->GetPointHeader());
    fMutframe->FillStat();
    int framesize = fMutframe->GetFrameSize();

    if ((fVerbose > 1) && InfoCondition()) {
        fMutframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fMutframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadHelloFrame(MFMCommonFrame* commonframe) {

    fHelloframe->SetAttributs(commonframe->GetPointHeader());
    fHelloframe->FillStat();
    int framesize = fHelloframe->GetFrameSize();

    if ((fVerbose > 1) && InfoCondition()) {
        fHelloframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fHelloframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadOscilloFrame(MFMCommonFrame* commonframe) {

    fOscilloframe->SetAttributs(commonframe->GetPointHeader());
    fOscilloframe->FillStat();
    int framesize = fOscilloframe->GetFrameSize();
    if ((fVerbose > 3) && InfoCondition()) {
        fOscilloframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fOscilloframe->DumpRaw(dump, 0);
    }
}
//_______________________________________________________________________________________________________________________
void ReadChimeraFrame(MFMCommonFrame* commonframe) {

    fChimeraframe->SetAttributs(commonframe->GetPointHeader());
    fChimeraframe->FillStat();
    int framesize = fChimeraframe->GetFrameSize();
    if ((fVerbose > 3) && InfoCondition()) {
        fChimeraframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fChimeraframe->DumpRaw(dump, 0);
    }
}

//_______________________________________________________________________________________________________________________
void ReadXmlDataDescriptionFrame(MFMCommonFrame* commonframe) {
    fDatadescriptionframe->SetAttributs(commonframe->GetPointHeader());
    fDatadescriptionframe->FillStat();
    int framesize = fDatadescriptionframe->GetFrameSize();

    if ((fVerbose > 1) && InfoCondition()) {
        fDatadescriptionframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fDatadescriptionframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadXmlHeaderFrame(MFMCommonFrame* commonframe) {
    fHeaderframe->SetAttributs(commonframe->GetPointHeader());
    fHeaderframe->FillStat();
    int framesize = fHeaderframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fHeaderframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fHeaderframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadBoxDiagFrame(MFMCommonFrame* commonframe) {

    fBoxDiagframe->SetAttributs(commonframe->GetPointHeader());
    fBoxDiagframe->FillStat();
    int framesize = fBoxDiagframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fBoxDiagframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fBoxDiagframe->DumpRaw(dump, 0);
    }
}
//_______________________________________________________________________________________________________________________
void ReadVamosICFrame(MFMCommonFrame* commonframe) {

    fVamosICframe->SetAttributs(commonframe->GetPointHeader());
    fVamosICframe->FillStat();
    int framesize = fVamosICframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fVamosICframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fVamosICframe->DumpRaw(dump, 0);
    }
}

//_______________________________________________________________________________________________________________________
void ReadVamosPDFrame(MFMCommonFrame* commonframe) {
    fVamosPDframe->SetAttributs(commonframe->GetPointHeader());
    fVamosPDframe->FillStat();
    int framesize = fVamosPDframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fVamosPDframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fVamosPDframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadVamosTACFrame(MFMCommonFrame* commonframe) {

    fVamosTACframe->SetAttributs(commonframe->GetPointHeader());
    fVamosTACframe->FillStat();
    int framesize = fVamosTACframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fVamosTACframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fVamosTACframe->DumpRaw(dump, 0);
    }
}
//_______________________________________________________________________________________________________________________
void ReadNedaFrame(MFMCommonFrame* commonframe) {

    fNedaframe->SetAttributs(commonframe->GetPointHeader());
    fNedaframe->FillStat();
    int framesize = fNedaframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fNedaframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fNedaframe->DumpRaw(dump, 0);
    }
}
//_______________________________________________________________________________________________________________________
void ReadNedaCompFrame(MFMCommonFrame* commonframe) {

    fNedaCompframe->SetAttributs(commonframe->GetPointHeader());
    fNedaCompframe->FillStat();
    int framesize = fNedaCompframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fNedaCompframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fNedaCompframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadS3BaF2Frame(MFMCommonFrame* commonframe) {

    fS3BaF2frame->SetAttributs(commonframe->GetPointHeader());
    fS3BaF2frame->FillStat();
    int framesize = fS3BaF2frame->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fS3BaF2frame->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3BaF2frame->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadS3AlphaFrame(MFMCommonFrame* commonframe) {

    fS3Alphaframe->SetAttributs(commonframe->GetPointHeader());
    fS3Alphaframe->FillStat();
    int framesize = fS3Alphaframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fS3Alphaframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3Alphaframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadS3RuthFrame(MFMCommonFrame* commonframe) {

    fS3Ruthframe->SetAttributs(commonframe->GetPointHeader());
    fS3Ruthframe->FillStat();
    int framesize = fS3Ruthframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fS3Ruthframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3Ruthframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadS3eGUNFrame(MFMCommonFrame* commonframe) {

    fS3eGUNframe->SetAttributs(commonframe->GetPointHeader());
    fS3eGUNframe->FillStat();
    int framesize = fS3eGUNframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fS3eGUNframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3eGUNframe->DumpRaw(dump, 0);
    }
}
///_______________________________________________________________________________________________________________________
void ReadS3SynchroFrame(MFMCommonFrame* commonframe) {

    fS3Synchroframe->SetAttributs(commonframe->GetPointHeader());
    fS3Synchroframe->FillStat();
    int framesize = fS3Synchroframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fS3Synchroframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3Synchroframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadReaGenericFrame(MFMCommonFrame* commonframe) {

    fReaGenericframe->SetAttributs(commonframe->GetPointHeader());
    fReaGenericframe->FillStat();
    int framesize = fReaGenericframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fReaGenericframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fReaGenericframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void ReadReaScopeFrame(MFMCommonFrame* commonframe) {

    fReaScopeframe->SetAttributs(commonframe->GetPointHeader());
    fReaScopeframe->FillStat();
    int framesize = fReaScopeframe->GetFrameSize();
    if ((fVerbose > 1) && InfoCondition()) {
        fReaScopeframe->HeaderDisplay();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fReaScopeframe->DumpRaw(dump, 0);
    }

}
//_______________________________________________________________________________________________________________________
void WriteUserFrame(int lun, int format, int fNbFrames, int fNbSubFrames) {

    switch (format) {

    case 1: {
        WriteCoboFrame(lun, fNbFrames, false);
        break;
    }
    case 2: {
        WriteCoboFrame(lun, fNbFrames, true);
        break;
    }
        //_____________________Exogam____________________________________________________________
    case 3: {
        WriteExogamFrame(lun, fNbFrames);
        break;
    }
    case 4: {
        //_____________________  Oscilloscope________________________________________________
        WriteOscilloFrame(lun, fNbFrames);
        break;
    }
        //_____________________ NEDA Raw____________________________________________________________
    case 5: {
        WriteNedaFrame(lun, fNbFrames);
        break;
    }
        //_____________________ NEDA Compressed_____________________________________________________
    case 6: {
        WriteNedaCompFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Ebyedat event number_____________________________________________________
    case 7: {
        int type = MFM_EBY_EN_FRAME_TYPE;
        WriteEbyedatFrame(lun, fNbFrames, type);
        break;
    }

        //_____________________ Ebyedat Time stamp ____________
    case 8: {
        int type = MFM_EBY_TS_FRAME_TYPE;
        WriteEbyedatFrame(lun, fNbFrames, type);
        break;
    }
        //_____________________ Ebyedat event number and time stamp______________________________________________________
    case 9: {
        int type = MFM_EBY_EN_TS_FRAME_TYPE;
        WriteEbyedatFrame(lun, fNbFrames, type);
        break;
    }
        //_____________________ Scaler data frame______________________________________________________
    case 10: {
        WriteScalerFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Scaler data frame______________________________________________________
    case 11: {
        WriteMutantFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Ribf data frame______________________________________________________
    case 12: {
        WriteRibfFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Hello frame______________________________________________________
    case 13: {
        WriteHelloFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Box Diagnostic frame______________________________________________________
    case 14: {
        WriteBoxDiagFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Vamos IC frame______________________________________________________
    case 15: {
        WriteVamosICFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Vamos PD  frame______________________________________________________
    case 16: {
        WriteVamosPDFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Vamos PD  frame______________________________________________________
    case 17: {
        WriteVamosTACFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Diamant frame______________________________________________________
    case 18: {
        WriteDiamantFrame(lun, fNbFrames);
        break;
    }
        //_____________________ S3BaF2F frame______________________________________________________
    case 19: {
        WriteS3BaF2Frame(lun, fNbFrames);
        break;
    }
        //_____________________ S3Alpha frame______________________________________________________
    case 20: {
        WriteS3AlphaFrame(lun, fNbFrames);
        break;
    }
        //_____________________ S3Ruth frame______________________________________________________
    case 21: {
        WriteS3RuthFrame(lun, fNbFrames);
        break;
    }
        //_____________________ Diamant frame______________________________________________________
    case 22: {
        WriteS3eGUNFrame(lun, fNbFrames);
        break;
    }
        //_____________________ S3Synchro frame______________________________________________________
    case 23: {
        WriteS3SynchroFrame(lun, fNbFrames);
        break;
    }
        //_____________________ ReaGeneric frame______________________________________________________
    case 24: {
        WriteReaGenericFrame(lun, fNbFrames);
        break;
    }
        //_____________________ ReaScope frame______________________________________________________
    case 25: {
        WriteReaScopeFrame(lun, fNbFrames);
        break;
    }
        //_____________________ XmlDataDescriptionFrame frame______________________________________________________
    case 26: {
        WriteXmlDataDescriptionFrame(lun, fNbFrames);
        break;
    }		//_____________________Merge of  Ebyedat in event number_____________________________________________________
    case 99: {
        int type;

        WriteXmlDataDescriptionFrame(lun, 1);

        WriteCoboFrame(lun, fNbFrames, false);
        WriteCoboFrame(lun, fNbFrames, true);
        WriteExogamFrame(lun, fNbFrames);
        WriteOscilloFrame(lun, fNbFrames);
        WriteNedaFrame(lun, fNbFrames);
        WriteNedaCompFrame(lun, fNbFrames);
        WriteEbyedatFrame(lun, fNbFrames, MFM_EBY_EN_FRAME_TYPE);
        WriteEbyedatFrame(lun, fNbFrames, MFM_EBY_TS_FRAME_TYPE);
        WriteEbyedatFrame(lun, fNbFrames, MFM_EBY_EN_TS_FRAME_TYPE);
        WriteScalerFrame(lun, fNbFrames);
        WriteMutantFrame(lun, fNbFrames);
        WriteRibfFrame(lun, fNbFrames);
        WriteHelloFrame(lun, fNbFrames);
        WriteBoxDiagFrame(lun, fNbFrames);
        WriteVamosICFrame(lun, fNbFrames);
        WriteVamosPDFrame(lun, fNbFrames);
        WriteVamosTACFrame(lun, fNbFrames);
        WriteDiamantFrame(lun, fNbFrames);
        WriteS3BaF2Frame(lun, fNbFrames);
        WriteS3AlphaFrame(lun, fNbFrames);
        WriteS3RuthFrame(lun, fNbFrames);
        WriteS3eGUNFrame(lun, fNbFrames);
        WriteS3SynchroFrame(lun, fNbFrames);
        WriteReaGenericFrame(lun, fNbFrames);
        WriteReaScopeFrame(lun, fNbFrames);

        break;
    }
        //_____________________Merge of  Ebyedat in event number_____________________________________________________
    case 100: {
        int type = MFM_EBY_EN_FRAME_TYPE;
        WriteMergeFrame(lun, fNbFrames, type, fNbSubFrames);
        break;
    }
        //_____________________ Merge of  Ebyedat in  time stamp______________________________________________________
    case 101: {
        int type = MFM_EBY_TS_FRAME_TYPE;
        WriteMergeFrame(lun, fNbFrames, type, fNbSubFrames);
        break;
    }
    case 102: {
        int type = MFM_COBO_FRAME_TYPE;
        WriteMergeFrame(lun, fNbFrames, type, fNbSubFrames);
        break;
    }
    case 200: {
        ConversionAdonisToFrames(lun);
        break;
    }

    default:
        cout << "Error : This format is not understood\n\n";
        exit(0);
        break;

    }// end of swich

}

//_______________________________________________________________________________________________________________________
void WriteXmlDataDescription(int flun, int nbframe) {

}

//_______________________________________________________________________________________________________________________
void WriteExogamFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = EXO_STD_UNIT_BLOCK_SIZE;
    framesize = EXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fExoframe->MFM_make_header(unitBlock_size, 0, MFM_EXO2_FRAME_TYPE,
                               revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fExoframe->FillEventRandomConst(timestamp, fEventNumber);
        //fExoframe->FillEventRandomConst(fEventNumber, fEventNumber);
        fExoframe->FillStat();
        if (fVerbose > 1)
            fExoframe->HeaderDisplay((char*) "-- Header of Exogam Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fExoframe->GetPointHeader() << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fExoframe->DumpRaw(dump, 0);

        verif = write(flun, fExoframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}
//_______________________________________________________________________________________________________________________
void WriteBoxDiagFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = NUMEXO_STD_UNIT_BLOCK_SIZE;
    framesize = NUMEXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fBoxDiagframe->MFM_make_header(unitBlock_size, 0, MFM_BOX_DIAG_FRAME_TYPE,
                                   revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fBoxDiagframe->FillEventRandomConst(timestamp, fEventNumber);
        fBoxDiagframe->FillStat();
        if (fVerbose > 1)
            fBoxDiagframe->HeaderDisplay(
                        (char*) "-- Header of Box Diag Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fBoxDiagframe->GetPointHeader()
                 << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fBoxDiagframe->DumpRaw(dump, 0);

        verif = write(flun, fBoxDiagframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}
//_______________________________________________________________________________________________________________________
void WriteVamosICFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = VAMOSIC_STD_UNIT_BLOCK_SIZE;
    framesize = VAMOSIC_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fVamosICframe->MFM_make_header(unitBlock_size, 0, MFM_VAMOSIC_FRAME_TYPE,
                                   revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fVamosICframe->FillEventRandomConst(timestamp, fEventNumber);
        fVamosICframe->FillStat();
        if (fVerbose > 1)
            fVamosICframe->HeaderDisplay(
                        (char*) "-- Header of Vamos IC Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fVamosICframe->GetPointHeader()
                 << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fVamosICframe->DumpRaw(dump, 0);

        verif = write(flun, fVamosICframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}
//_______________________________________________________________________________________________________________________
void WriteVamosPDFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = VAMOSPD_STD_UNIT_BLOCK_SIZE;
    framesize = VAMOSPD_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fVamosPDframe->MFM_make_header(unitBlock_size, 0, MFM_VAMOSPD_FRAME_TYPE,
                                   revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i * 4;
        fEventNumber = i;
        fVamosPDframe->FillEventRandomConst(timestamp, fEventNumber);
        fVamosPDframe->FillStat();
        if (fVerbose > 1)
            fVamosPDframe->HeaderDisplay(
                        (char*) "-- Header of Vamos PD Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fVamosPDframe->GetPointHeader()
                 << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fVamosPDframe->DumpRaw(dump, 0);

        verif = write(flun, fVamosPDframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }

}
//_______________________________________________________________________________________________________________________
void WriteVamosTACFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = VAMOSTAC_STD_UNIT_BLOCK_SIZE;
    framesize = VAMOSTAC_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fVamosTACframe->MFM_make_header(unitBlock_size, 0, MFM_VAMOSTAC_FRAME_TYPE,
                                    revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fVamosTACframe->FillEventRandomConst(timestamp, fEventNumber);
        fVamosTACframe->FillStat();
        if (fVerbose > 1)
            fVamosTACframe->HeaderDisplay(
                        (char*) "-- Header of TAC Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fVamosTACframe->GetPointHeader()
                 << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fVamosTACframe->DumpRaw(dump, 0);

        verif = write(flun, fVamosTACframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}
//_______________________________________________________________________________________________________________________
void WriteNedaFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t itemsize = 0;
    uint32_t nbitem = 0;
    uint32_t framesize = 0;
    uint32_t revision = 1;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    unitBlock_size = NEDA_STD_UNIT_BLOCK_SIZE;
    itemsize = sizeof(MFM_Neda_Item);
    nbitem = NEDA_NB_OF_ITEMS;
    int verif;

    headersize = NEDA_HEADERSIZE;

    framesize = headersize + nbitem * itemsize + sizeof(MFM_Neda_EOF);
    revision = 1;
    fNedaframe->SetEndFrame(1);// introduce 1% of error in end frame.
    // generation of MFM header
    fNedaframe->MFM_make_header(unitBlock_size, 0, MFM_NEDA_FRAME_TYPE,
                                revision, (int) (framesize / unitBlock_size), (headersize
                                                                               / unitBlock_size), itemsize, nbitem);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = fNedaframe->GetTimeStampUs();
        fEventNumber = i;

        fNedaframe->SetLocationId(1, 2);
        fNedaframe->SetLeInterval(3);
        fNedaframe->SetZcoInterval(4);
        fNedaframe->SetTdcValue(5);
        fNedaframe->SetSlowIntegral(6);
        fNedaframe->SetFastIntegral(7);
        fNedaframe->SetBitfield(8);
        fNedaframe->SetAbsMax(9);
        fNedaframe->FillEventWithRamdomConst(timestamp, fEventNumber);
        fNedaframe->FillEndOfFrame();
        fNedaframe->FillStat();
        if (fVerbose > 1)
            fNedaframe->HeaderDisplay((char*) "-- Header of Neda Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fNedaframe->GetPointHeader() << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fNedaframe->DumpRaw(dump, 0);

        verif = write(flun, fNedaframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }

}
//_______________________________________________________________________________________________________________________
void WriteNedaCompFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = NEDACOMP_STD_UNIT_BLOCK_SIZE;
    uint32_t framesize = NEDACOMP_FRAMESIZE;
    uint32_t revision = 1;
    uint32_t headersize = NEDACOMP_HEADERSIZE;
    uint64_t timestamp = 0;
    int verif;

    revision = 1;
    // generation of MFM header
    fNedaCompframe->MFM_make_header(unitBlock_size, 0, MFM_NEDACOMP_FRAME_TYPE,
                                    revision, (int) (framesize / unitBlock_size), (headersize
                                                                                   / unitBlock_size));

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = fNedaCompframe->GetTimeStampUs();
        fEventNumber = i;

        fNedaCompframe->SetLocationId(1, 112);

        fNedaCompframe->FillEventWithRamdomConst(timestamp, fEventNumber);
        fNedaCompframe->FillStat();
        if (fVerbose > 1)
            fNedaCompframe->HeaderDisplay(
                        (char*) "-- Header of Neda Comp Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fNedaCompframe->GetPointHeader()
                 << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fNedaCompframe->DumpRaw(dump, 0);

        verif = write(flun, fNedaCompframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }

}

//_______________________________________________________________________________________________________________________
void WriteDiamantFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = NUMEXO_STD_UNIT_BLOCK_SIZE;
    framesize = NUMEXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames

    fDiamantframe->MFM_make_header(unitBlock_size, 0, MFM_DIAMANT_FRAME_TYPE,
                                   revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = fDiamantframe->GetTimeStampUs();

        fEventNumber = i;
        fDiamantframe->FillEventRandomConst(timestamp, fEventNumber);
        fDiamantframe->FillStat();
        if (fVerbose > 1)
            fDiamantframe->HeaderDisplay(
                        (char*) "-- Header of Diamant Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fDiamantframe->GetPointHeader()
                 << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fDiamantframe->DumpRaw(dump, 0);

        verif = write(flun, fDiamantframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }

}
//_______________________________________________________________________________________________________________________
void WriteS3BaF2Frame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = NUMEXO_STD_UNIT_BLOCK_SIZE;
    framesize = NUMEXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fS3BaF2frame->MFM_make_header(unitBlock_size, 0, MFM_S3_BAF2_FRAME_TYPE,
                                  revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fS3BaF2frame->FillEventRandomConst(timestamp, fEventNumber);
        fS3BaF2frame->FillStat();
        if (fVerbose > 1)
            fS3BaF2frame->HeaderDisplay((char*) "-- Header of S3 BaF2 Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fS3BaF2frame->GetPointHeader() << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3BaF2frame->DumpRaw(dump, 0);

        verif = write(flun, fS3BaF2frame->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }

}

//_______________________________________________________________________________________________________________________
void WriteS3AlphaFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;
    unitBlock_size = NUMEXO_STD_UNIT_BLOCK_SIZE;
    framesize = NUMEXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fS3Alphaframe->MFM_make_header(unitBlock_size, 0, MFM_S3_ALPHA_FRAME_TYPE,
                                   revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fS3Alphaframe->FillEventRandomConst(timestamp, fEventNumber);
        fS3Alphaframe->FillStat();
        if (fVerbose > 1)
            fS3Alphaframe->HeaderDisplay(
                        (char*) "-- Header of S3 Si Alpha Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fS3Alphaframe->GetPointHeader()
                 << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3Alphaframe->DumpRaw(dump, 0);

        verif = write(flun, fS3Alphaframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}
//_______________________________________________________________________________________________________________________
void WriteS3RuthFrame(int flun, int fNbFrames) {
    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;
    unitBlock_size = NUMEXO_STD_UNIT_BLOCK_SIZE;
    framesize = NUMEXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fS3Ruthframe->MFM_make_header(unitBlock_size, 0, MFM_S3_RUTH_FRAME_TYPE,
                                  revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fS3Ruthframe->FillEventRandomConst(timestamp, fEventNumber);
        fS3Ruthframe->FillStat();
        if (fVerbose > 1)
            fS3Ruthframe->HeaderDisplay(
                        (char*) "-- Header of Si Rutherford Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fS3Ruthframe->GetPointHeader() << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3Ruthframe->DumpRaw(dump, 0);

        verif = write(flun, fS3Ruthframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}
//_______________________________________________________________________________________________________________________
void WriteS3eGUNFrame(int flun, int fNbFrames) {
    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = NUMEXO_STD_UNIT_BLOCK_SIZE;
    framesize = NUMEXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fS3eGUNframe->MFM_make_header(unitBlock_size, 0, MFM_S3_EGUN_FRAME_TYPE,
                                  revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fS3eGUNframe->FillEventRandomConst(timestamp, fEventNumber);
        fS3eGUNframe->FillStat();
        if (fVerbose > 1)
            fS3eGUNframe->HeaderDisplay((char*) "-- Header of S3 eGUN Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fS3eGUNframe->GetPointHeader() << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3eGUNframe->DumpRaw(dump, 0);

        verif = write(flun, fS3eGUNframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}
//_______________________________________________________________________________________________________________________
void WriteS3SynchroFrame(int flun, int fNbFrames) {
    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = NUMEXO_STD_UNIT_BLOCK_SIZE;
    framesize = NUMEXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fS3Synchroframe->MFM_make_header(unitBlock_size, 0, MFM_S3_SYNC_FRAME_TYPE,
                                     revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fS3Synchroframe->FillEventRandomConst(timestamp, fEventNumber);
        fS3Synchroframe->FillStat();
        if (fVerbose > 1)
            fS3Synchroframe->HeaderDisplay(
                        (char*) "-- Header of Synchro Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fS3Synchroframe->GetPointHeader()
                 << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fS3Synchroframe->DumpRaw(dump, 0);

        verif = write(flun, fS3Synchroframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}

//_______________________________________________________________________________________________________________________
void WriteReaGenericFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;

    int verif;


    unitBlock_size = NUMEXO_STD_UNIT_BLOCK_SIZE;
    framesize = NUMEXO_FRAMESIZE;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fReaGenericframe->MFM_make_header(unitBlock_size, 0, MFM_REA_GENE_FRAME_TYPE,
                                      revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fReaGenericframe->FillEventRandomConst(timestamp, fEventNumber);
        fReaGenericframe->FillStat();
        if (fVerbose > 1)
            fReaGenericframe->HeaderDisplay((char*) "-- Header of Rea Generic Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fReaGenericframe->GetPointHeader() << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fReaGenericframe->DumpRaw(dump, 0);

        verif = write(flun, fReaGenericframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}


//_______________________________________________________________________________________________________________________
void WriteReaScopeFrame(int flun, int fNbFrames) {

    uint32_t unitBlock_size = 0;
    uint32_t itemsize = 0;
    uint32_t nbitem = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;
    headersize = REA_SCOPE_HEADERSIZE;
    unitBlock_size = REA_SCOPE_STD_UNIT_BLOCK_SIZE;
    framesize = sizeof(MFM_ReaScope_Frame);
    revision = 1;
    itemsize = sizeof(MFM_ReaScope_Item);
    nbitem = REA_SCOPE_NB_OF_ITEMS;


    // generation of MFM header , in this case, MFM is same for all MFM frames
    fReaScopeframe->MFM_make_header(unitBlock_size, 0, MFM_REA_SCOPE_FRAME_TYPE,
                                    revision, (int) (framesize / unitBlock_size), (headersize
                                                                                   / unitBlock_size), itemsize, nbitem);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fReaScopeframe->FillEventWithRamdomConst(timestamp, fEventNumber);
        fReaScopeframe->FillStat();
        if (fVerbose > 1)
            fReaScopeframe->HeaderDisplay((char*) "-- Header of Rea Scope Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fReaScopeframe->GetPointHeader() << "\n";
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fReaScopeframe->DumpRaw(dump, 0);

        verif = write(flun, fReaScopeframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}

//_______________________________________________________________________________________________________________________
void WriteCoboFrame(int lun, int fNbFrames, bool full) {
    int32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;
    int type;
    if (full == false)
        type = MFM_COBO_FRAME_TYPE;
    else
        type = MFM_COBOF_FRAME_TYPE;

    for (int i = 0; i < fNbFrames; i++) {
        fCoboframe->GenerateACoboExample(type, i, fEventNumber % COBO_NB_ASAD);
        if (fVerbose > 1) {
            if (full)
                fCoboframe->HeaderDisplay(
                            (char*) "-- Header of CoboFull Frame --");
            else
                fCoboframe->HeaderDisplay((char*) "-- Header of Cobo Frame --");
        }
        framesize = fCoboframe->GetFrameSize();
        fCoboframe->FillStat();
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fCoboframe->DumpRaw(dump, 0);

        verif = write(lun, fCoboframe->GetPointHeader(), framesize);
        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");
        fEventNumber++;

    }

}
//_______________________________________________________________________________________________________________________
void WriteOscilloFrame(int lun, int fNbFrames) {
    int32_t unitBlock_size = 0;
    uint32_t itemsize = 0;
    uint32_t nbitem = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    itemsize = sizeof(MFM_OscilloItem);
    int datasize;
    revision = 1;
    unitBlock_size = OSCILLO_STD_UNIT_BLOCK_SIZE;
    nbitem = 16384;// 16384 is usual size for numexo2
    headersize = MFM_OSCILLO_HEADERSIZE;
    framesize = headersize + nbitem * itemsize;
    revision = 1;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fOscilloframe->MFM_make_header(unitBlock_size, 0, MFM_OSCI_FRAME_TYPE,
                                   revision, (int) (framesize / unitBlock_size), (headersize
                                                                                  / unitBlock_size), itemsize, nbitem);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;

        fOscilloframe->FillEventWithRamp(1, 1);
        fOscilloframe->FillStat();
        if (fVerbose > 5)
            cout << " pointheader = " << fOscilloframe->GetPointHeader()
                 << "\n";

        if (fVerbose > 1)
            fOscilloframe->HeaderDisplay(
                        (char*) "-- Header of Oscillo Frame --");
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fOscilloframe->DumpRaw(dump, 0);

        verif = write(lun, fOscilloframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }

}

//_______________________________________________________________________________________________________________________

void WriteEbyedatFrame(int lun, int fNbFrames, int type) {
    uint32_t eventnum;
    int32_t unitBlock_size = 0;
    uint32_t itemsize = 0;
    uint32_t nbitem = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;


    char info[64];
    if (type == MFM_EBY_EN_FRAME_TYPE)
        strcpy(info, "-- Header of Ebyedat event number Frame --");
    if (type == MFM_EBY_EN_TS_FRAME_TYPE)
        strcpy(info,
               "-- Header Ebyedat of event number and time stamp Frame --");
    if (type == MFM_EBY_TS_FRAME_TYPE)
        strcpy(info, "-- Header Ebyedat of event time stamp Frame --");

    for (int i = 0; i < fNbFrames; i++) {
        fEbyframe->GenerateAEbyedatExample(type, i);
        framesize = fEbyframe->GetFrameSize();
        fEbyframe->FillStat();

        if (fVerbose > 1)
            fEbyframe->HeaderDisplay(info);

        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fEbyframe->DumpRaw(dump, 0);

        verif = write(lun, fEbyframe->GetPointHeader(), framesize);
        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");
    }
}

//_______________________________________________________________________________________________________________________

void WriteScalerFrame(int lun, int fNbFrames) {
    int type = MFM_SCALER_DATA_FRAME_TYPE;

    time_t seconds_past_epoch;
    uint64_t ts;
    uint32_t eventnum;
    int32_t unitBlock_size = 0;
    uint32_t itemsize = 0;
    uint32_t nbitem = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;
    int nb_item = 20;
    for (int i = 0; i < fNbFrames; i++) {
        seconds_past_epoch = time(0);
        ts = (uint64_t) seconds_past_epoch;
        fScalerframe->GenerateAScalerExample(ts, i, nb_item);
        framesize = fScalerframe->GetFrameSize();
        fScalerframe->FillStat();
        if (fVerbose > 1)
            fScalerframe->HeaderDisplay((char*) "-- Header Scaler Frame --");
        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fScalerframe->DumpRaw(dump, 0);
        verif = write(lun, fScalerframe->GetPointHeader(), framesize);
        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");
    }

}
//_______________________________________________________________________________________________________________________

void WriteMutantFrame(int lun, int fNbFrames) {
    int32_t unitBlock_size = 0;
    uint32_t itemsize = 0;
    uint32_t nbitem = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = MUT_UNIT_BLOCK_SIZE;

    framesize = sizeof (MFM_mut_frame);
    revision = 0;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fMutframe->MFM_make_header(unitBlock_size, 0, MFM_MUTANT_FRAME_TYPE,
                               revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fMutframe->FillEventRandomConst(timestamp, fEventNumber);
        framesize = fMutframe->GetFrameSize();
        fMutframe->FillStat();
        if (fVerbose > 1)
            fMutframe->HeaderDisplay((char*) "-- Header Mutant Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fMutframe->GetPointHeader() << "\n";

        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fMutframe->DumpRaw(dump, 0);

        verif = write(lun, fMutframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");
        fEventNumber++;
        timestamp = fEventNumber;
    }

}
//_______________________________________________________________________________________________________________________
void WriteRibfFrame(int lun, int fNbFrames) {

    int32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;
    unitBlock_size = 2;
    framesize = 64;
    revision = 0;
    // generation of MFM header , in this case, MFM is same for all MFM frames
    fRibfframe->MFM_make_header(unitBlock_size, 0, MFM_RIBF_DATA_FRAME_TYPE,
                                revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fRibfframe->FillEventRandomConst(timestamp, fEventNumber);
        framesize = fRibfframe->GetFrameSize();
        fRibfframe->FillStat();
        if (fVerbose > 3)
            fRibfframe->HeaderDisplay((char*) "-- Header Ribf Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fRibfframe->GetPointHeader() << "\n";

        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fRibfframe->DumpRaw(dump, 0);

        verif = write(lun, fRibfframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");
        fEventNumber++;
        timestamp = fEventNumber;
    }

}

//_______________________________________________________________________________________________________________________

void WriteHelloFrame(int lun, int fNbFrames) {
    int32_t unitBlock_size = 0;;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = 1;
    framesize = HEL_FRAMESIZE;
    revision = 0;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fHelloframe->MFM_make_header(unitBlock_size, 0, MFM_HELLO_FRAME_TYPE,
                                 revision, (int) (framesize / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fHelloframe->FillEventRandomConst(timestamp, fEventNumber);

        framesize = fHelloframe->GetFrameSize();
        fHelloframe->FillStat();
        if (fVerbose > 3)
            fHelloframe->HeaderDisplay((char*) "-- Header Hello Frame --");
        if (fVerbose > 5)
            cout << " pointheader = " << fHelloframe->GetPointHeader() << "\n";

        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fHelloframe->DumpRaw(dump, 0);

        verif = write(lun, fHelloframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }

}

//_______________________________________________________________________________________________________________________
void WriteXmlDataDescriptionFrame(int lun, int fNbFrames) {
    int32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;

    unitBlock_size = 4;
    framesize = MFM_BLOB_HEADER_SIZE;
    revision = 0;

    // generation of MFM header , in this case, MFM is same for all MFM frames
    fDatadescriptionframe->MFM_make_header(unitBlock_size, 0,
                                           MFM_XML_DATA_DESCRIPTION_FRAME_TYPE, revision, (int) (framesize
                                                                                                 / unitBlock_size), true);

    // generation of fNbFrames frame
    for (int i = 0; i < fNbFrames; i++) {
        timestamp = i;
        fEventNumber = i;
        fDatadescriptionframe->FillEventRandomConst();

        framesize = fDatadescriptionframe->GetFrameSize();
        fDatadescriptionframe->FillStat();
        if (fVerbose > 3)
            fDatadescriptionframe->HeaderDisplay(
                        (char*) "-- Header XML Data Description Frame --");
        if (fVerbose > 5)
            cout << " pointheader = "
					<< fDatadescriptionframe->GetPointHeader() << "\n";

        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fDatadescriptionframe->DumpRaw(dump, 0);

        verif = write(lun, fDatadescriptionframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");

        fEventNumber++;
        timestamp = fEventNumber;

    }
}

//_______________________________________________________________________________________________________________________
void WriteMergeFrame(int lun, int fNbFrames, int type, int fNbSubFrames) {
    uint32_t eventnum;
    int32_t unitBlock_size = 0;
    uint32_t framesize = 0;
    uint32_t revision = 0;
    uint32_t headersize = 0;
    uint64_t timestamp = 0;
    int verif;
    uint32_t nbitem = fNbSubFrames;
    char info[64];

    unitBlock_size = 2;
    fEventNumber = 0;

    framesize = 0;
    revision = 1;
    bool cobotag = false;
    int merge_type;

    MFMCommonFrame *insideframe[nbitem];

    if (type == MFM_EBY_TS_FRAME_TYPE) {
        headersize = MERGE_TS_HEADERSIZE;
        merge_type = MFM_MERGE_TS_FRAME_TYPE;
        strcpy(info, "--Header Ebyedat  with TS--");
    }

    if (type == MFM_EBY_EN_FRAME_TYPE) {
        headersize = MERGE_EN_HEADERSIZE;
        merge_type = MFM_MERGE_EN_FRAME_TYPE;
        strcpy(info, "-- Header Ebyedat with EN--");
    }

    if (type == MFM_EBY_EN_TS_FRAME_TYPE) {
        headersize = MERGE_TS_HEADERSIZE;
        merge_type = MFM_MERGE_TS_FRAME_TYPE;
        strcpy(info, "-- Header Ebyedat with TS and EN--");
    }

    if (type == MFM_COBO_FRAME_TYPE) {
        cobotag = true;
        headersize = MERGE_EN_HEADERSIZE;
        merge_type = MFM_MERGE_EN_FRAME_TYPE;
        strcpy(info, "-- Header COBO --");
        for (int j = 0; j < nbitem; j++) {
            insideframe[j] = (MFMCommonFrame*) (new MFMCoboFrame());
        }
    } else {
        MFMEbyedatFrame * ebyframe2p[nbitem];
        for (int j = 0; j < nbitem; j++) {
            insideframe[j] = (MFMCommonFrame*) (new MFMEbyedatFrame());
        }
    }
    // generation of fNbFrames contents
    for (int i = 0; i < fNbFrames; i++) {
        fEventNumber++;
        framesize = headersize;
        int framesizelocal;
        for (int j = 0; j < nbitem; j++) {
            if (cobotag) {
                ((MFMCoboFrame*) insideframe[j])->GenerateACoboExample(type,
                                                                       fEventNumber, nbitem % COBO_NB_ASAD);
                fCoboframe->SetAttributs(insideframe[j]->GetPointHeader());
                framesizelocal = fCoboframe->GetFrameSize();
                fCoboframe->FillStat();
            } else {
                ((MFMEbyedatFrame*) insideframe[j])->GenerateAEbyedatExample(
                            type, fEventNumber);
                fEbyframe->SetAttributs(insideframe[j]->GetPointHeader());
                framesizelocal = fEbyframe->GetFrameSize();
                fEbyframe->FillStat();
            }
            framesize += insideframe[j]->GetFrameSize();
            fInsideframe->SetAttributs(insideframe[j]->GetPointHeader());
            framesizelocal = fInsideframe->GetFrameSize();
            fInsideframe->FillStat();
        }
        cout << "-- Layered Frame (Merge Frame) -with " << nbitem
             << " inside frames , so have a look to inside frames first------\n";
        fMergeframe->MFM_make_header(unitBlock_size, 0, merge_type, revision,
                                     (int) (framesize / unitBlock_size), (headersize
                                                                          / unitBlock_size), 0, nbitem);
        fMergeframe->SetAttributs();
        fMergeframe->ResetAdd();
        for (int j = 0; j < nbitem; j++) {
            int insframesize = insideframe[j]->GetFrameSizeAttribut();
            cout << " ---- inside frame ---- ";
            fMergeframe->AddFrame(insideframe[j]);
            if (fVerbose > 1)
                insideframe[j]->HeaderDisplay((char*) info);
            int dump = fDumpsize;
            if (insframesize < dump)
                dump = insframesize;
            if (fVerbose > 3)
                insideframe[j]->DumpRaw(dump, 0);

        }
        fMergeframe->SetEventNumber(fEventNumber);
        fMergeframe->FillStat();
        if (fVerbose > 1)
            fMergeframe->HeaderDisplay((char*) "-- Header of  Merge Frame --");

        int dump = fDumpsize;
        if (framesize < dump)
            dump = framesize;
        if (fVerbose > 3)
            fMergeframe->DumpRaw(dump, 0);

        verif = write(lun, fMergeframe->GetPointHeader(), framesize);

        if (verif != framesize)
            Error.TreatError(2, 0, "Error of write");
    }
    for (int j = 0; j < nbitem; j++) {
        if (insideframe[j]) delete insideframe[j];
    }

}

//_______________________________________________________________________________________________________________________
void ConversionAdonisToFrames(int lun) {
    int sizeoftrace = 4048;
    int readsize;
    string filename;
    uint32_t eventnum;
    int verif;
    char* vector, *vector1, *vector2;
    char ** pvector;
    vector = (char*) (malloc(sizeoftrace));
    pvector = &vector;
    vector1 = (char*) (malloc(sizeoftrace));
    vector2 = (char*) (malloc(sizeoftrace));
    int type = MFM_EBY_EN_FRAME_TYPE;
    int fLun2;
    filename = "mfmfile2.dat";
    MFMEbyedatFrame * ebyframe = new MFMEbyedatFrame();
    cout << "File name to convert in buffer of " << sizeoftrace << " points?\n";
    //cin >> filename;
    cout << "filename  == " << (filename.data()) << "\n";
    fLun2 = open(filename.data(), (O_RDONLY));
    if (lun <= 0) {
        Error.TreatError(2, 0, "Error open file :", filename.data());
        exit(0);
    }
    uint32_t unitBlock_size = 0;
    uint32_t itemsize = 0;
    uint32_t nbitem = 0;
    uint32_t framesize = 0;
    uint32_t revision = 1;
    uint32_t headersize = EBYEDAT_EN_HEADERSIZE;
    uint16_t label = 0;
    uint16_t value = 0;
    unitBlock_size = 2;
    itemsize = 4;
    int fEventNumber = 0;
    cout << " begin of transformation\n";
    while (true) {
        readsize = read(fLun2, (void*) (vector), sizeoftrace);

        cout << " readsize = " << readsize << " " << sizeoftrace << "\n";
        if (readsize > 0) {
            cout << "." << flush;
            nbitem = readsize;
            ebyframe->MFM_make_header(unitBlock_size, 0, type, revision,
                                      readsize, (headersize / unitBlock_size), itemsize, nbitem);
            ebyframe->SetEventNumber(fEventNumber++);
            for (int i = 0; i < nbitem; i++) {
                label = ((char*) vector)[i] >> 14;
                value = ((char*) vector)[i] & ADONIS_MSK;
                ebyframe->EbyedatSetParameters(i, label, value);
                cout << "label = " << label << " value= " << value << "\n";
            }
            if (true) {
                ebyframe->HeaderDisplay(
                            (char*) "-- Header of Ebyedat event number Frame --");
                cout << "TimeStamp = " << ebyframe->GetTimeStamp() << "\n";
                eventnum = (unsigned long long) ebyframe->GetEventNumber();
                cout << "EbventNumber = " << eventnum << "\n";
                framesize = ebyframe->GetFrameSize();
                ebyframe->DumpRaw(64, 0);
            }
            verif = write(lun, ebyframe->GetPointHeader(), framesize);
            if (verif != framesize)
                Error.TreatError(2, 0, "Error of write");
        }
        if (readsize != sizeoftrace)
            break;

    }

    free(vector);

}
//_______________________________________________________________________________________________________________________
void Statistics(bool writeorread) {
    if (fVerbose > 0) {
        cout << endl;
        cout << "----------------------------------------------" << endl;
        if (writeorread)
            cout << "|             WRITE STATISTICS                |" << endl;
        else
            cout << "|             READ STATISTICS                 |" << endl;
        cout << "----------------------------------------------" << endl;

        if (fHeaderframe->GetCountFrame() != 0) {
            fHeaderframe->PrintStat("Data Header Xml");
            cout << "----------------------------------------------" << endl;
        }
        if (fDatadescriptionframe->GetCountFrame() != 0) {
            fDatadescriptionframe->PrintStat("Data Description");
            cout << "----------------------------------------------" << endl;
        }

        if (fCoboframe->GetCountFrame() != 0) {
            fCoboframe->PrintStat("Cobo");
            cout << "----------------------------------------------" << endl;
        }
        if (fCobotopoframe->GetCountFrame() != 0) {
            fCobotopoframe->PrintStat("CoboTopo");
            cout << "----------------------------------------------" << endl;
        }
        if (fEbyframe->GetCountFrame() != 0) {
            fEbyframe->PrintStat("EbyEdat");
            cout << "----------------------------------------------" << endl;
        }

        if (fExoframe->GetCountFrame() != 0) {
            fExoframe ->PrintStat("Exogam");
            cout << "----------------------------------------------" << endl;
        }
        if (fBoxDiagframe->GetCountFrame() != 0) {
            fBoxDiagframe ->PrintStat("Box  Diagnostic");
            cout << "----------------------------------------------" << endl;
        }
        if (fVamosICframe->GetCountFrame() != 0) {
            fVamosICframe ->PrintStat("Vamos Ionization Chamber");
            cout << "----------------------------------------------" << endl;
        }
        if (fVamosTACframe->GetCountFrame() != 0) {
            fVamosTACframe ->PrintStat("Vamos TAC");
            cout << "----------------------------------------------" << endl;
        }
        if (fVamosPDframe->GetCountFrame() != 0) {
            fVamosPDframe ->PrintStat("Vamos Position Detector");
            cout << "----------------------------------------------" << endl;
        }
        if (fNedaframe->GetCountFrame() != 0) {
            fNedaframe ->PrintStat("NEDA");
            cout << "----------------------------------------------" << endl;
        }
        if (fNedaCompframe->GetCountFrame() != 0) {
            fNedaCompframe ->PrintStat("NEDA Compressed");
            cout << "----------------------------------------------" << endl;
        }
        if (fDiamantframe->GetCountFrame() != 0) {
            fDiamantframe ->PrintStat("Diamant");
            cout << "----------------------------------------------" << endl;
        }
        if (fS3BaF2frame->GetCountFrame() != 0) {
            fS3BaF2frame ->PrintStat("S3 BaF2");
            cout << "----------------------------------------------" << endl;
        }
        if (fS3Alphaframe->GetCountFrame() != 0) {
            fS3Alphaframe ->PrintStat("S3 Alpha");
            cout << "----------------------------------------------" << endl;
        }
        if (fS3Ruthframe->GetCountFrame() != 0) {
            fS3Ruthframe ->PrintStat("Rutherford");
            cout << "----------------------------------------------" << endl;
        }
        if (fS3eGUNframe->GetCountFrame() != 0) {
            fS3eGUNframe ->PrintStat("S3 eGUN");
            cout << "----------------------------------------------" << endl;
        }
        if (fS3Synchroframe->GetCountFrame() != 0) {
            fS3Synchroframe ->PrintStat("S3 Synchro");
            cout << "----------------------------------------------" << endl;
        }
        if (fScalerframe->GetCountFrame() != 0) {
            string text = "Scaler";
            fScalerframe-> PrintStat(text);
            cout << "----------------------------------------------" << endl;
        }
        if (fOscilloframe->GetCountFrame() != 0) {
            string text = "Oscillo";
            fOscilloframe-> PrintStat(text);
            cout << "----------------------------------------------" << endl;
        }
        if (fRibfframe->GetCountFrame() != 0) {
            fRibfframe-> PrintStat("RIBF");
            cout << "----------------------------------------------" << endl;
        }
        if (fMutframe->GetCountFrame() != 0) {
            fMutframe->PrintStat("MUTANT");
            cout << "----------------------------------------------" << endl;
        }
        if (fHelloframe->GetCountFrame() != 0) {
            fHelloframe->PrintStat("HELLO");
            cout << "----------------------------------------------" << endl;
        }
        if (fMergeframe->GetCountFrame() != 0) {
            fMergeframe->PrintStat("Merge");
            fInsideframe->PrintStat("InsideFrame");
        }

        cout << "----------------------------------------------" << endl;
        cout << "Number of ElseFrame   :" << fCount_elseframe << endl;

        cout << "\n";

    }
}
//_______________________________________________________________________________________________________________________
