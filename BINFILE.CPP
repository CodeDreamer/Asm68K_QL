/***********************************************************************
 *
 *		BINFILE.CPP
 *		Teeside binary file routines for 68000 Assembler
 *              This file contains the routines that generate a binary
 *              file compatible with the Teeside 68000 emulator. I am
 *              not sure about the header configuration of the Teeside
 *              binary files. Several of the bytes in the header appear
 *              to be constant (perhaps version number info etc.) and
 *              are simply copied from a Teeside binary file.
 *
 *    Function: initBin()
 *		Opens the specified binary code file for writing. If
 *		the file cannot be opened, then the routine prints a
 *		message and exits.
 *
 *		outputBin()
 *		Places the data whose size, value, and address are
 *		specified in the binary code file.
 *
 *		finishBin()
 *              Closes the object code file and writes the Teeside header.
 *
 *      Author: Chuck Kelly
 *              Jan-28-2002
 *              Monroe County Community College
 *              http://www.monroeccc.edu/ckelly
 *
 ************************************************************************/


// include <vcl.h>
#include <stdio.h>
#include <ctype.h>
// include "mainS.h"
// include "textS.h"
#include "asm.h"

// prototype
void writeBigEndian(int data, int size);

extern char line[256];
extern FILE *binFile;
extern char newOrg;
extern char buffer[256];  //ck used to form messages for display in windows
extern char numBuf[20];
extern bool offsetMode;
extern bool isRelative;

int binAddr;                    // address of the current file position
int binBase;                    // address of the first data byte in the file
int binEnd;                     // address just past the last data byte written
long binHeaderSize;             // bytes of QDOS header before the data
bool newFile = false;
//unsigned int sectionCount;     // number of sections
//unsigned int byteCount;        // code bytes in current program section
//int byteCountFP;               // byte count file position
//int currentFP;                 // save current file position
extern unsigned int startAddress;     // starting address of program
//int fileSize;                  // count bytes in file
int execDataSize;


int initBin(const char *name)
{
  binFile = fopen(name, "wb");  // create binary file
  if (!binFile) {
    sprintf(buffer,"Unable to create binary file!");
    //Application->MessageBox(buffer, "Error", MB_OK);
      fprintf(stderr,"%s\n",buffer);
    return MILD_ERROR;
  }

  newFile = true;
  //sectionCount = 0;                     // # of sections
  //byteCount = 0;                        // clear section byte count
  binAddr = binBase = binEnd = 0;       // default address
  binHeaderSize = 0;

  execDataSize = 500;

//  fileSize = 0;                         // 0 bytes in header
//  fileSize = 20;                        // 20 bytes in header

  // write Teeside header to file (removed in EASy68K v4.4.4)
  //writeBigEndian(0x0001, WORD_SIZE);         // version # ?
  //writeBigEndian(0, LONG_SIZE);              // # of sections (update later)
  //writeBigEndian(0, LONG_SIZE);              // unknown
  //writeBigEndian(0, LONG_SIZE);              // starting address (update later)
  //writeBigEndian(0x2800, WORD_SIZE);         // unknown
  //writeBigEndian(0, LONG_SIZE);              // unknown

  return NORMAL;
}

int outputBin(int newAddr, int data, int size)
{
  int zero = 0;
  static const char* headString = "]!QDOS File Header";

  try {
      if (offsetMode)       // don't write data if processing Offset directive
        return NORMAL;

      // init starting address of new file
      if (newFile)
      {
        //binAddr = newAddr;
        newFile = false;
        //sectionCount++;                 // count sections
        //writeBigEndian(newAddr, LONG_SIZE);  // write new address to file
        //byteCountFP = ftell(binFile);   // save file position of byteCount
        //writeBigEndian(0, LONG_SIZE);        // save room for byte count (update later)
        //fileSize += 8;                      // keep track of file size

        if (execDataSize) {
            unsigned char buff[22] = { 0 };
            strcpy((char*)buff, headString);
            buff[19] = 15;  // Header size in words
            buff[21] = 1;   // QDOS executable flag

            fwrite(buff, 1, sizeof(buff), binFile);
            writeBigEndian(execDataSize, LONG_SIZE);
            writeBigEndian(0, LONG_SIZE);
            binHeaderSize = sizeof(buff) + 2 * LONG_SIZE;
        }

        // Relocatable code starts at address 0; absolute code (ORG)
        // starts at the address of its first data
        binAddr = binBase = binEnd = isRelative ? 0 : newAddr;
      }

      // if writing WORD_SIZE or LONG_SIZE data to odd address
      if ((size != BYTE_SIZE) && (newAddr % 2)) {
          newAddr++;                          // next address
      }

      if (newAddr < binBase) {
          fprintf(stderr, "Binary output: data at $%X is before the start of the file ($%X)\n", newAddr, binBase);
          return MILD_ERROR;
      }

      // ORG moved back into data already written: overwrite it
      if (newAddr < binAddr || (newAddr > binAddr && newAddr <= binEnd)) {
          fseek(binFile, binHeaderSize + (newAddr - binBase), SEEK_SET);
          binAddr = newAddr;
      }

      // If the new address is past the end of the data, pad with zeros
      if (newAddr > binAddr) {
          fseek(binFile, 0, SEEK_END);
          binAddr = binEnd;
        //currentFP = ftell(binFile);          // save current file position
        // position file at previous sections byte count
        //fseek(binFile, byteCountFP, SEEK_SET);
        //writeBigEndian(byteCount, LONG_SIZE);     // write previous sections byte count
        //fseek(binFile, currentFP, SEEK_SET); // return file to current position
        //byteCount = 0;                       // clear byte count for new section

        //sectionCount++;                 // count sections
        //writeBigEndian(newAddr, LONG_SIZE);  // write new address to file
        //byteCountFP = ftell(binFile);   // save file position of byteCount
        //writeBigEndian(0, LONG_SIZE);        // save room for byte count (update later)

          while (binAddr + 4 <= newAddr) {
              fwrite(&zero, 1, 4, binFile);
              binAddr += 4;
          }

          if (newAddr > binAddr)
              fwrite(&zero, 1, newAddr - binAddr, binFile);

        binAddr = newAddr;
        //fileSize += 8;                      // keep track of file size
      }

      writeBigEndian(data, size);           // write the data to the file
      //byteCount += (int) size;             // count the bytes
      binAddr += (int) size;               // next address
      if (binAddr > binEnd)
          binEnd = binAddr;
      //fileSize += size;                     // keep track of file size
  }
  catch( ... ) {
    //sprintf(buffer, "ERROR: An exception occurred in routine 'outputBin'. \n");
    printError(NULL, EXCEPTION, 0);
    return MILD_ERROR;
  }
  return NORMAL;
}

int finishBin()
{
  try {
  // make sure file contains an even number of bytes
  if ((binEnd - binBase) % 2) {         // if odd size
    fseek(binFile, 0, SEEK_END);
    writeBigEndian(0, 1);               // write an extra byte of 0
  }

  //currentFP = ftell(binFile);          // save current file position
  //// position file at previous sections byte count
  //fseek(binFile, byteCountFP, SEEK_SET);
  //writeBigEndian(byteCount, LONG_SIZE);     // write previous sections byte count

  //rewind(binFile);                      // rewind to start of file

  // write Teeside header to file  removed in EASy68K v4.4.4
  //writeBigEndian(0x0001, WORD_SIZE);         // version # ?
  //writeBigEndian(sectionCount, LONG_SIZE);   // # of sections
  //writeBigEndian(0, LONG_SIZE);              // unknown
  //writeBigEndian(startAddress, LONG_SIZE);   // starting address
  //writeBigEndian(0x2800, WORD_SIZE);         // unknown
  //writeBigEndian(0, LONG_SIZE);              // unknown

  // close the file
  fclose(binFile);
  }
  catch( ... ) {
    //sprintf(buffer, "ERROR: An exception occurred in routine 'finishBin'. \n");
    printError(NULL, EXCEPTION, 0);
    return MILD_ERROR;
  }

  return NORMAL;
}

// Write size bytes from data to file in Big-Endian format
// Always writes WORD_SIZE and LONG_SIZE data to even address.
void writeBigEndian(int data, int size)
{
  unsigned char dataByte;

  try {
  // if writing WORD_SIZE or LONG_SIZE data to odd address
  //if ((size != BYTE_SIZE) && (ftell(binFile) % 2))
  //{
  //  dataByte = 0;
  //  fwrite(&dataByte, 1, 1, binFile);   // pad with byte of 0
  //}

      // write data to file
      for (int i=size-1; i>=0; i--)
      {
        dataByte = data >> 8*i;
        fwrite(&dataByte, 1, 1, binFile);
      }
  }
  catch( ... ) {
    sprintf(buffer, "ERROR: An exception occurred in routine 'writeBigEndian'. \n");
    printError(NULL, EXCEPTION, 0);
  }
}


