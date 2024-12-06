#include <iostream>
using namespace std;
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <tinyxml.h>
#include "MFMAllFrames.h"
#include "DataParameters.h"
#include "DataScalers.h"
#include "MError.h"
#include "Cobo.h"
#include <cstring>
#include <cstdlib>
#include <time.h>
#include <sys/stat.h>
MFMCommonFrame * fFrame;

MError Error;
int fDumpsize;
int fDumpPbsize;
int fMaxdump;
int fNonExpectedSizeCount;
int fExpectedSize;
int fExpectedType;
int fNbFrames;
int fNbSubFrames;
uint32_t fEventNumber;
int fVerbose;
long long fFileSize = 0;
int fFormat;
long long fReadsize = 0;
int fMaxSizeForFrame = 2048;
const char * fDeffilename = "mfmfile.dat";
const char * fDeffileoutname = "mfmfileout.dat";
#define ADONIS_MSK 0x3FFF
int ReadInBadFile(FILE *fLun, char** vector, int * vectorsize,
		int * readframesize);
void ReadDefaultFrame(MFMCommonFrame* commonframe);
void ClearVector(char* vector, int vectorsize);
void Statistics();
void Help();
void ErrorMessage();

int main(int argc, char **argv) {

	char* tempo[128];
	fDumpsize = 16;
	fExpectedSize = 52;
	fMaxdump = 128;
	fFormat = 0;
	int fNbFrames = 20;
	fNbSubFrames = 5;
	char *tempos, *wordtempo;
	bool writeorread = false;
	int treatedinfo = 0;
	bool infonbframe = false;
	fNonExpectedSizeCount = 0;
	bool all = false;
	fVerbose = 5;
	fReadsize = 0;
	fDumpPbsize = 0;
	bool output = false;
	bool extype = false;
	char filename[255];
	char fileoutname[255];
	fMaxSizeForFrame = 2048;
	strcpy(filename, fDeffilename);
	strcpy(fileoutname, fDeffileoutname);
	fFrame = new MFMCommonFrame();
	struct stat filestatus;

	fFrame->InitStat();

	for (int i = 1; i < argc; i++) {

		treatedinfo = 0;

		if ((strcasecmp(argv[i], "-h") == 0) or (strcasecmp(argv[i], "--help")
				== 0)) {
			Help();
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
			infonbframe = true;
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
		if ((strcasecmp(wordtempo, "-o") == 0) or (strcasecmp(wordtempo,
				"--output") == 0)) {
			wordtempo = strtok(NULL, "=");

			if (wordtempo == NULL)
				ErrorMessage();
			if (strcasecmp(wordtempo, "") != 0)
				strcpy(fileoutname, wordtempo);
			output = true;
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
		if ((strcasecmp(wordtempo, "-dp") == 0) or (strcasecmp(wordtempo,
				"--dumppb") == 0)) {
			fDumpPbsize = 1;
			treatedinfo++;
		}

		if ((strcasecmp(wordtempo, "-a") == 0) or (strcasecmp(wordtempo,
				"--all") == 0)) {
			all = true;
			treatedinfo++;
		}

		if ((strcasecmp(wordtempo, "-es") == 0) or (strcasecmp(wordtempo,
				"--expectedsize") == 0)) {
			wordtempo = strtok(NULL, "=");
			if (wordtempo == NULL)
				ErrorMessage();
			if (strcasecmp(wordtempo, "") == 0)
				ErrorMessage();
			fExpectedSize = atoi(wordtempo);
			treatedinfo++;
		}
		if ((strcasecmp(wordtempo, "-et") == 0) or (strcasecmp(wordtempo,
				"--expectedtype") == 0)) {
			wordtempo = strtok(NULL, "=");
			if (wordtempo == NULL)
				ErrorMessage();
			if (strcasecmp(wordtempo, "") == 0)
				ErrorMessage();
			fExpectedType = atoi(wordtempo);
			extype = true;
			treatedinfo++;
		}

		if ((strcasecmp(wordtempo, "-ms") == 0) or (strcasecmp(wordtempo,
				"--maxsize") == 0)) {
			wordtempo = strtok(NULL, "=");
			if (wordtempo == NULL)
				ErrorMessage();
			if (strcasecmp(wordtempo, "") == 0)
				ErrorMessage();
			fMaxSizeForFrame = atoi(wordtempo);
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

		if (treatedinfo == 0) {
			ErrorMessage();
			return (0);
		}

	}
	if (argc == 1) {
		Help();
		return (0);
	}

	//_____________________ READ____________________________________________________________


	cout << endl;
	cout << "-------------------------------------------------------------"
			<< endl;
	cout << "|   TEST  MFMFrame from file " << filename << "" << endl;
	cout << "-------------------------------------------------------------"
			<< endl;

	fMaxdump = 128;
	FILE* fLun = NULL; // Logical Unit Number
	FILE* fLunOut = NULL;

	stat(filename, &filestatus);
	fFileSize = filestatus.st_size;

	fLun = fopen(filename, "r");

	if (fFileSize < fMaxSizeForFrame)
		fMaxSizeForFrame = fFileSize;

	if (fLun == NULL) {
		Error.TreatError(2, 0, "Error open file :", filename);
	}
	if (output) {
		fLunOut = fopen(fileoutname, "w");
		if (fLunOut == NULL) {
			Error.TreatError(2, 0, "Error open file :", fileoutname);
		}
	}
	int minsizeheader = MFM_BLOB_HEADER_SIZE; // =8
	int cont = 0;
	int nbofevent = 0;

	char* vector;
	char ** pvector;
	int vectorsize = fMaxSizeForFrame;
	vector = (char*) (malloc(vectorsize));
	pvector = &vector;
	int framesize = 0;
	int retour;
	int readframesize = 0;
	int type;
	int fisrtretour = 0;
	while (true) {
		ClearVector(vector, vectorsize);
		retour = ReadInBadFile(fLun, pvector, &vectorsize, &readframesize);
		if (retour == 0)
			break;
		fFrame->SetAttributs(vector);
		type = fExpectedType;
		framesize = fFrame->GetFrameSizeAttribut();
		if (extype) {
			type = fFrame->GetFrameTypeAttribut();
		}

		if ((framesize != fExpectedSize) || (type != fExpectedType)) {
			if (fDumpPbsize > 0) {
				cout << " ----------Dump Problem------" << fReadsize
						<< "------------\n";
				fFrame->DumpRaw(fExpectedSize, 0);
			}
			fReadsize++;
			fNonExpectedSizeCount++;
			if (fisrtretour == 1) {
				fseek(fLun, (-retour + 1), SEEK_CUR);
			} else {
				fseek(fLun, (-retour - 4), SEEK_CUR);
				fisrtretour = 0;
			}

		} else {
			fisrtretour = 1;
			fFrame->FillStat();
			fReadsize += retour;
			if (output)
				fwrite(vector, 1, retour, fLunOut);
		}

		if (fVerbose > 1) {
			cout << "\n-- Read frame in file : nb = " << nbofevent
					<< " ----position in file :" << fReadsize
					<< "-----------------------------------------\n";
		}
		framesize = retour;
		ReadDefaultFrame(fFrame);
		nbofevent++;
		fFrame->SetAttributs();
	}

	Statistics();

	fclose(fLun);
	if (fLunOut)
		fclose(fLunOut);

	if (fFrame)
		delete (fFrame);

}
//--------------------------------------------------------------------------------------------------------
void Help() {

	cout
			<< "\n MFMFileTest.exe : utility to scan a file and try to find MFM inside  \n";
	cout << " Usage  : \n";
	cout << "          MFMFileTest.exe [-h -f -d -v -es -et -o]      \n";
	cout << "            -h or --help , print this help \n";
	cout
			<< "            -f=filname or --file=filename , set name of file to read or to write , (default  \""
			<< fDeffilename << "\"\n";
	cout
			<< "            -es=nb or --expectedsize=nb, set the expected size of frame (decimal integer) (default : "
			<< fExpectedSize << ")\n";
	cout
			<< "            -et=nb or --expectedtype=nn, set the expected type of frame (decimal integer) (default : "
			<< fExpectedType << ")   \n";
	cout
			<< "            -ms=nb or --maxsize=nb, set the maxsize for  frame (decimal integer)  (default : "
			<< fMaxSizeForFrame << ") \n";
	cout
			<< "            -v=nb or --verbose=nb, give more information during read or write(default : "
			<< fVerbose << ") \n";
	cout
			<< "            -d=nb or --dump=nb,  number of bytes to dump.(default : "
			<< fDumpsize << ") \n";
	cout << "            -dp or --dumppb,  dump  only when a problem arrives\n";
	cout
			<< "            -o=fileoutputname or --output=fileoutputname, outputfile containing good frames.(default : "
			<< fDeffileoutname << ") \n";
	cout << endl;
	cout
			<< "   Examples MFMFileTest.exe  -f=MyFile.dat : Read and test file MyFile.dat\n ";
	cout << endl;
}

//--------------------------------------------------------------------------------------------------------
void ErrorMessage() {
	cout << " Command not good, see help with MFMFileTest.exe -h\n";
	exit(0);
}
//--------------------------------------------------------------------------------------------------------
void ClearVector(char* vector, int vectorsize) {
	int i;
	for (i = 0; i < vectorsize; i++)
		vector[i] = 0;
}
//--------------------------------------------------------------------------------------------------------
void ReadDefaultFrame(MFMCommonFrame* commonframe) {

	if (fVerbose > 1) {
		commonframe->HeaderDisplay(
				(char*) ((commonframe->WhichFrame()).c_str()));
		if (fVerbose > 3) {
			int framesize = commonframe->GetFrameSize();
			int dump = fDumpsize;
			if (framesize < dump)
				dump = framesize;
			if (dump < 4)
				dump = 4;

			if (dump > (fFileSize - fReadsize))
				dump = fFileSize - fReadsize;

			commonframe->DumpRaw(dump, 0);
		}
	}
}

//____________________________________________________________________
int ReadInBadFile(FILE * fLun, char** vector, int * vectorsize,
		int * readframesize) {
	/// Get data from a file, and fill current frame and initialize its attributs and its pointer
	/// if size of actual frame is not enough, and new size is reallocated
	///      fLun   : descriptor of file (given by a previous open)
	///      vector : pointer on pointer will contain frame . if size isn't big enouth, a new value of pointer
	///       and vectorsize of this pointer
	///      return size of read .

	int countsize = 0;
	char* vectornew = NULL;
	int FirstSizeToRead = 4;
	int sizetoread = 0;

	countsize = fread((void*) (*vector), 1, FirstSizeToRead, fLun);

	if (countsize <= 0) {
		cout << " End of read file\n";
		return countsize;
	}
	if (countsize < FirstSizeToRead) {
		if (FirstSizeToRead < (fFileSize - fReadsize))
			cout << " Error in read file ! \n";
		return countsize;
	}

	fFrame->SetAttributsOn4Bytes((*vector));
	*readframesize = fFrame->GetFrameSizeAttribut();
	sizetoread = *readframesize;

	if (*readframesize < fMaxSizeForFrame)
		sizetoread = *readframesize;
	else
		sizetoread = fMaxSizeForFrame;

	if (sizetoread < fExpectedSize)
		sizetoread = fExpectedSize;

	if ((*vectorsize < sizetoread)) {
		vectornew = (char*) (realloc((void*) (*vector), sizetoread));
		if (vectornew != NULL) {
			(*vector) = vectornew;
			*vectorsize = sizetoread;
			fFrame->SetAttributs((*vector));
		} else {
			Error.TreatError(1, sizetoread,
					" Memory allocation in MFMCommonFrame:: ReadInFile");
			return 0;
		}
	}

	sizetoread -= FirstSizeToRead;

	countsize = fread((void*) ((*vector) + FirstSizeToRead), 1, sizetoread,
			fLun);
	if (countsize != sizetoread) {
		if (sizetoread < (fFileSize - (fReadsize + FirstSizeToRead))) {
			char info[200];
			sprintf(info, "Error in read file ,read /asked to read : %d / %d ",
					countsize, sizetoread);
			Error.TreatError(1, 0, info);
		}
	}
	return countsize + FirstSizeToRead;
}

//--------------------------------------------------------------------------------------------------------
void Statistics() {
	if (fVerbose > 0) {
		cout << endl;
		cout << "----------------------------------------------" << endl;
		cout << "|             READ STATISTICS                 |" << endl;
		cout << "----------------------------------------------" << endl;
		cout << "-----------Stat on good frames----------------" << endl;
		if (fFrame->GetCountFrame() != 0) {
			fFrame->PrintStat("MFM frame");

		}
		cout << endl;
		cout << "-----------Stat on not good frames------------" << endl;
		cout << " Non expected size = " << fNonExpectedSizeCount << "\n";
		cout << "---------------------------------------------" << endl;
	}
}
