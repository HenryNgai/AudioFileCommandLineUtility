#include <stdlib.h>

#include "debug.h"
#include "hw1.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the content of three frames of audio data and
 * two annotation fields have been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 1 if validation succeeds and 0 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variables "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 1 if validation succeeds and 0 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv){
    int argsParsed=1; // default has 1 arg parsed bin/audible
    int factor;
    int keyCounter=0;
    unsigned long tempShift = 0x1;


    if (argc==argsParsed){
        debug("No arguments");
        return returnFailure();
    }

    if (compareStrings(*(argv+argsParsed),"-h")){
            debug("registered h \n");
            global_options = global_options ^(tempShift <<63);
            return 1;
    }


    if (compareStrings(*(argv+argsParsed),"-u") || compareStrings(*(argv+argsParsed),"-d")){
        if(compareStrings(*(argv+argsParsed),"-u")){
            debug("Detected u flag");
             global_options = global_options ^(tempShift <<62);
        }
        if(compareStrings(*(argv+argsParsed),"-d")){
            debug("Detected d flag");
            global_options = global_options ^(tempShift <<61);
        }
        argsParsed++;
        if(argsParsed == argc){ // If there is no more arguments, success.
            debug("Last argument");
            return 1;
        }

        if(compareStrings(*(argv+argsParsed),"-f")){
            debug("Detected f flag");
            argsParsed++;
            if (argsParsed == argc){
                debug("No factor entered after f flag. Defualt value of factor is 1");
                return 1;
            }

            factor = myAtoi(*(argv+argsParsed));
            if (factor<1 || factor>1024){// If factor is less than one or greater than 1024 fail
                debug("Factor is invalid");
                return returnFailure();
            }

            unsigned long tempFactor = factor;
            global_options = global_options ^ (tempFactor-1)<<48;

            debug("Detected Valid Factor");
            argsParsed++;
            if(argsParsed == argc){
                debug("No more flags");
                return 1;
            }
            if(compareStrings(*(argv+argsParsed),"-p")){
                debug("Detected p Flag");
                global_options = global_options ^(tempShift <<59);
                //Preserve input annotations without modifications
            }
            argsParsed++;
            if (argsParsed == argc){
                debug("There are more arguments. Valid");
                return 1;
            }
            debug("There is another argument after invalid");
            return 0;
        }


        if(compareStrings(*(argv+argsParsed),"-p")){
            debug("Detected p Flag");
            global_options = global_options ^(tempShift <<59);
            //Preserve input annotations without modifications
        }
            argsParsed++;
        if (argsParsed == argc){
            debug("There are no more arguments. Valid");
            return 1;
        }

        if(compareStrings(*(argv+argsParsed),"-f")){
        debug("Detected f flag");
        argsParsed++;
        if (argsParsed == argc){
            debug("No factor entered after f flag. Default factor is 1");
            return 1;
        }

        factor = myAtoi(*(argv+argsParsed));
        if (factor<1 || factor>1024){// If factor is less than one or greater than 1024 fail
            debug("Factor is invalid");
            return returnFailure();
        }

        unsigned long tempFactor = factor;
        global_options = global_options ^ (tempFactor-1)<<48;

        debug("Detected Valid Factor");
        argsParsed++;
        if(argsParsed == argc){
            debug("No more flags");
            return 1;
        }
        debug("There is another argument after. Invalid");
        return returnFailure();
        }

        debug("There are more arguments but it is not a -f flag");
        return returnFailure();
    }


    if (compareStrings(*(argv+argsParsed),"-c")) {
        global_options = global_options ^(tempShift <<60);
        debug("Registered c");
        argsParsed++;
        if(argsParsed==argc){
            debug("No Flag after (K is required)");
            return returnFailure();
        }
         if(compareStrings(*(argv+argsParsed),"-k")){
            debug("Detected k flag");
            argsParsed++;
            if(argsParsed==argc){
                debug("No key after k flag");
                return returnFailure();
            }

            char* temp = *(argv+argsParsed);
            int checkRange;
            for(;*(temp) != '\0';keyCounter++,temp++){}
            if (keyCounter>8){
                debug("Greater than 8 character for key. Failed");
                return returnFailure();
            }
            temp=*(argv+argsParsed);
            for(checkRange=0;checkRange<keyCounter;checkRange++){
                if(*(temp+checkRange)<48){ // If its less than 0 fail.
                    debug ("Key less than 0. Failed");
                    return returnFailure();
                }
                if(*(temp+checkRange)>57){ // If greater than 9, check to see if it is A-F or a-f
                    if(!((*(temp+checkRange)>=65 && *(temp+checkRange)<=70)||(*(temp+checkRange)>=97
                        && *(temp+checkRange)<=102))){
                        debug("key not between A-F or a-f");
                        return returnFailure();
                    }
                }
            }
            debug("Key was verified");
            global_options = global_options ^ (myStroul(temp));
            argsParsed++;
            if(argsParsed==argc)
                return 1;

            if((compareStrings(*(argv+argsParsed),"-p"))){
                debug ("Registered p flag after key ");
                global_options = global_options ^(tempShift <<59);
                //Preserve input annotations without modifications;
            }
            else{
                debug("Flag detected after key but it was not P.");
                return returnFailure();
            }
        }

        else if((compareStrings(*(argv+argsParsed),"-p"))){
            debug ("Detected -p flag");
            global_options = global_options ^(tempShift <<59);
            //Preserve input annotations without modifications;
            argsParsed++;
            if(argsParsed==argc){
                debug("No key flag detected (must have -k flag");
                return returnFailure();
            }

            if(compareStrings(*(argv+argsParsed),"-k")){
                debug("Detected k flag");
                argsParsed++;
                if(argsParsed==argc){
                    debug("No key after k flag");
                    return returnFailure();
                }

                char* temp = *(argv+argsParsed);
                int checkRange;
                for(;*(temp) != '\0';keyCounter++,temp++){}
                if (keyCounter>8){
                    debug("Greater than 8 character for key. Failed");
                    return returnFailure();
                }
                temp=*(argv+argsParsed);
                for(checkRange=0;checkRange<keyCounter;checkRange++){
                    if(*(temp+checkRange)<48){ // If its less than 0 fail.
                        debug ("Key less than 0. Failed");
                        return returnFailure();
                    }
                    if(*(temp+checkRange)>57){ // If greater than 9, check to see if it is A-F or a-f
                        if(!((*(temp+checkRange)>=65 && *(temp+checkRange)<=70)||(*(temp+checkRange)>=97
                        && *(temp+checkRange)<=102))){
                            debug("key not between A-F or a-f");
                            return returnFailure();
                        }
                    }
                }
                debug("Key was verified");
                global_options = global_options ^ (myStroul(temp));
                argsParsed++;
                if(argsParsed==argc)
                    return 1;
                else{
                    debug("Detected flag after key failure");
                    return returnFailure();
                }
            }
        }

        else{
            debug("No valid flag after c");
            return returnFailure();
        }
    }

    if(!(compareStrings(*(argv+1),"-c") || compareStrings(*(argv+1),"-u") || compareStrings(*(argv+1),"-d"))){
        debug("not -c, -u , or -d");
        return returnFailure();
    }

    return 1;
}

/**
 * @brief  Recodes a Sun audio (.au) format audio stream, reading the stream
 * from standard input and writing the recoded stream to standard output.
 * @details  This function reads a sequence of bytes from the standard
 * input and interprets it as digital audio according to the Sun audio
 * (.au) format.  A selected transformation (determined by the global variable
 * "global_options") is applied to the audio stream and the transformed stream
 * is written to the standard output, again according to Sun audio format.
 *
 * @param  argv  Command-line arguments, for constructing modified annotation.
 * @return 1 if the recoding completed successfully, 0 otherwise.
 */

int recode(char **argv) {
    AUDIO_HEADER header;
    AUDIO_HEADER* hp = &header;
    read_header(hp);
    read_annotation(input_annotation,hp->data_offset-24);


    unsigned long check= 0x1;
    unsigned long intervalCheck = 0x3FF;
    int flagU =(global_options & (check<<62)) >> 62;
    int flagD = (global_options & (check<<61)) >> 61;
    int factor = ((global_options & (intervalCheck <<48)) >> 48)+1;
    int flagC = (global_options & (check<<60))>>60;
    int key=(global_options & 0xffffffff);
    int flagP= (global_options &(check<<59));
    int numberofNulls=0;




    if(flagP==0){
        char** tempPoint1 = argv; // Pointers
        char* tempPoint2= *argv;
        int numberOfBytesCounter=0;

        while(*tempPoint1 != NULL){
            tempPoint2 = *tempPoint1;
            while(*tempPoint2 != '\0'){
                tempPoint2++;
                numberOfBytesCounter++;
            }
            if(*(tempPoint1+1) != NULL)
                numberOfBytesCounter++;
            tempPoint1++;

        }
        numberOfBytesCounter++;



        numberofNulls =(8 - (numberOfBytesCounter % 8));
        hp->data_offset =  hp->data_offset+ numberOfBytesCounter + numberofNulls;
        debug("offset %d", hp->data_offset);


    }

    int bytesPerFrame = hp->channels * (hp -> encoding-1);
    int numberOfFrames = hp->data_size / bytesPerFrame;

    if(flagU){
        hp-> data_size = (((numberOfFrames / factor)+1) * bytesPerFrame);
    }
    if(flagD){
        hp->data_size = (numberOfFrames + ((numberOfFrames -1 ) * (factor - 1)))* bytesPerFrame;
    }

    write_header(hp);

    char* temp = *argv;
     while(*argv != NULL){
            temp = *argv;
            while(*temp != '\0'){
                putchar(*temp);
                temp++;
            }
            if(*(argv+1) != NULL)
                putchar(' ');
            argv++;

        }
    putchar('\n');

    write_annotation(input_annotation,hp->data_offset-24);

    for(int i=0; i<numberofNulls;i++){
        putchar('\0');
    }
    debug("%d",numberofNulls);
    debug("Above is number of nulls");

    if(flagU != 0){
        int bytesPerFrame = hp->channels * (hp -> encoding-1);
        int numberOfFrames = hp->data_size / bytesPerFrame;

        for (int i=0;i<numberOfFrames;i++){
            read_frame((int*)input_frame,hp->channels,(hp->encoding)-1);
            if((i % factor) == 0)
                write_frame((int*)input_frame,hp->channels,(hp->encoding)-1);
        }
        return 1;
    }

    if(flagD !=0){
        int* prevFrame = (int*) previous_frame;
        int* inputFrame = (int*) input_frame;
        int* outputFrame = (int*)output_frame;

         int bytesPerFrame = hp->channels * (hp -> encoding-1);
        int numberOfFrames = hp->data_size / bytesPerFrame;

        read_frame((int*)previous_frame,hp->channels,(hp->encoding)-1);

        for(int i=1;i<numberOfFrames;i++){
            read_frame((int*)input_frame,hp->channels,(hp->encoding)-1);
            write_frame((int*)previous_frame,hp->channels,(hp->encoding)-1);
            for(int k=1;k<=factor-1;k++){

                int s1 = *prevFrame;
                int t1 = *input_frame;

                int s2= *(prevFrame+1);
                int t2= *(inputFrame+1);


                int newFrame1 = s1 + (t1 - s1) * k/factor;
                int newFrame2 = s2 + (t2 - s2) * k/factor;

                *outputFrame = newFrame1;
                *(outputFrame+1) = newFrame2;

                write_frame((int*)output_frame,hp->channels,(hp->encoding)-1);

            }
            write_frame((int*)prevFrame,hp->channels,(hp->encoding)-1);

        *prevFrame = *input_frame;
        *(prevFrame+1) = *(inputFrame+1);

        }
        return 1;
    }

    if(flagC != 0){
        int* inputFrame = (int*) input_frame;
        int* outputFrame = (int*)output_frame;
        int bytesPerFrame = hp->channels * (hp -> encoding-1);
        int numberOfFrames = hp->data_size / bytesPerFrame;

        mysrand(key);

        for(int i=0;i<numberOfFrames;i++){
            read_frame((int*)input_frame,hp->channels,(hp->encoding)-1);
            if(hp->channels > 1){
                int firstChannel = *(inputFrame);
                int secondChannel = *(inputFrame+1);
                firstChannel = firstChannel ^ myrand32();
                secondChannel = secondChannel ^ myrand32();
                *outputFrame = firstChannel;
                *(outputFrame+1)=secondChannel;
            }
            else{
                int firstChannel = *(input_frame);
                firstChannel = firstChannel ^ myrand32();
                *outputFrame = firstChannel;
            }
            write_frame((int*)outputFrame,hp->channels,(hp->encoding)-1);
        }

    return 1;
    }



    return returnFailure();
}

int read_header(AUDIO_HEADER *hp){
    unsigned int magicNumber;
    unsigned int offset;
    unsigned int size;
    unsigned int encode;
    unsigned int rate;
    unsigned int channel;

    unsigned int inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    if (inputByte != 0x2e){
        return 0;
    }
    magicNumber = inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    if (inputByte != 0x73){
        return 0;
    }
    magicNumber = magicNumber << (8);
    magicNumber = magicNumber ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    if (inputByte != 0x6e){
        return 0;
    }
    magicNumber = magicNumber << (8);
    magicNumber = magicNumber ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
    return 0;

    if (inputByte != 0x64){
        return 0;
    }

    magicNumber = magicNumber << (8);
    magicNumber = magicNumber ^ inputByte;


    hp->magic_number = magicNumber;


    inputByte = getchar();
    if (inputByte == EOF)
        return 0;
    offset = inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    offset = offset << (8);
    offset = offset ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    offset = offset << (8);
    offset = offset ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
    return 0;

    offset = offset << (8);
    offset = offset ^ inputByte;

    if(offset % 8 != 0){
        //debug ("Offset not divisible by 8"); // Do we have to check if it is 24 or 32 based on annotations
        return 0;
    }

    hp->data_offset = offset;
    //debug("%x\n",offset);
    //debug ("Data_Offset successfully added to struct");


    inputByte = getchar();
    if (inputByte == EOF)
        return 0;
    size = inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    size = size << (8);
    size = size ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    size = size << (8);
    size = size ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
    return 0;

    size = size << (8);
    size = size ^ inputByte;

    // What does the 0xffffffff mean and do i need to do anything with it.

    hp->data_size = size;
    //debug("%x\n",size);
    //debug ("Data_Size successfully added to struct");




    inputByte = getchar();
    if (inputByte == EOF)
        return 0;
    encode = inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    encode = encode << (8);
    encode = encode ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    encode = encode << (8);
    encode = encode ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
    return 0;

    encode = encode << (8);
    encode = encode ^ inputByte;

    if(encode == 2 ||encode == 3 ||encode == 4 ||encode == 5){
        hp->encoding = encode;
        //debug("%x\n",encode);
        //debug ("Data_Size successfully added to struct");
    }
    else return 0;


    inputByte = getchar();
    if (inputByte == EOF)
        return 0;
    rate = inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    rate = rate << (8);
    rate = rate ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    rate = rate << (8);
    rate = rate ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
    return 0;

    rate = rate << (8);
    rate = rate ^ inputByte;

    hp->sample_rate = rate;
    //debug("%x\n",rate);
    //debug("Sample_Rate successfully added to struct");


    inputByte = getchar();
    if (inputByte == EOF)
        return 0;
    channel = inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    channel = channel << (8);
    channel = channel ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
        return 0;

    channel = channel << (8);
    channel = channel ^ inputByte;

    inputByte = getchar();
    if (inputByte == EOF)
    return 0;

    channel = channel << (8);
    channel = channel ^ inputByte;

    if(channel == 2 || channel == 1){
        hp->channels = channel;
        //debug("%x\n",channel);
        //debug("Channels successfully added to struct");
    }
    else return 0;




return 1;
}





int write_header(AUDIO_HEADER *hp){
    unsigned int byte1;
    unsigned int byte2;
    unsigned int byte3;
    unsigned int byte4;
    unsigned int temp;

    temp = hp->magic_number;

    byte1 = temp & 0xff000000 ;
    byte1 = byte1 >> (24);


    byte2 = temp & 0xff0000 ;
    byte2 = byte2 >> (16);


    byte3 = temp & 0xff00 ;
    byte3 = byte3 >> (8);


    byte4 = temp & 0xff ;

    if(putchar(byte1)==EOF){
        return 0;
    }
    //debug("%x",byte1);
    //debug("Added first byte of magic_number");

    if(putchar(byte2)==EOF){
        return 0;
    }
    //debug("%x",byte2);
    //debug("Added second byte of magic_number");

    if(putchar(byte3)==EOF){
        return 0;
    }
    //debug("%x",byte3);
    //debug("Added third byte of magic_number");

    if(putchar(byte4)==EOF){
        return 0;
    }
   // debug("%x",byte4);
    //debug("Added fourth byte of magic_number");

    temp = hp->data_offset;

    byte1 = temp & 0xff000000 ;
    byte1 = byte1 >> (24);


    byte2 = temp & 0xff0000 ;
    byte2 = byte2 >> (16);


    byte3 = temp & 0xff00 ;
    byte3 = byte3 >> (8);


    byte4 = temp & 0xff ;

   if(putchar(byte1)==EOF){
        return 0;
    }
    //debug("%x",byte1);
    //debug("Added first byte of data_offset");

    if(putchar(byte2)==EOF){
        return 0;
    }
    //debug("%x",byte2);
    //debug("Added second byte of data_offset");

    if(putchar(byte3)==EOF){
        return 0;
    }
    //debug("%x",byte3);
    //debug("Added third byte of data_offset");

    if(putchar(byte4)==EOF){
        return 0;
    }
    //debug("%x",byte4);
    //debug("Added fourth byte of data_offset");


    temp = hp->data_size;

    byte1 = temp & 0xff000000 ;
    byte1 = byte1 >> (24);


    byte2 = temp & 0xff0000 ;
    byte2 = byte2 >> (16);


    byte3 = temp & 0xff00 ;
    byte3 = byte3 >> (8);


    byte4 = temp & 0xff ;

    if(putchar(byte1)==EOF){
        return 0;
    }
    //debug("%x",byte1);
    //debug("Added first byte of data_size");

    if(putchar(byte2)==EOF){
        return 0;
    }
    //debug("%x",byte2);
    //debug("Added second byte of data_size");

    if(putchar(byte3)==EOF){
        return 0;
    }
    //debug("%x",byte3);
    //debug("Added third byte of data_size");

    if(putchar(byte4)==EOF){
        return 0;
    }
    //debug("%x",byte4);
    //debug("Added fourth byte of data_size");



    temp = hp->encoding;

    byte1 = temp & 0xff000000 ;
    byte1 = byte1 >> (24);


    byte2 = temp & 0xff0000 ;
    byte2 = byte2 >> (16);


    byte3 = temp & 0xff00 ;
    byte3 = byte3 >> (8);


    byte4 = temp & 0xff ;

    if(putchar(byte1)==EOF){
        return 0;
    }
    //debug("%x",byte1);
    //debug("Added first byte of encoding");

    if(putchar(byte2)==EOF){
        return 0;
    }
    //debug("%x",byte2);
    //debug("Added second byte of encoding");

    if(putchar(byte3)==EOF){
        return 0;
    }
    //debug("%x",byte3);
    //debug("Added third byte of encoding");

    if(putchar(byte4)==EOF){
        return 0;
    }
    //debug("%x",byte4);
    //debug("Added fourth byte of encoding");


    temp = hp->sample_rate;

    byte1 = temp & 0xff000000 ;
    byte1 = byte1 >> (24);


    byte2 = temp & 0xff0000 ;
    byte2 = byte2 >> (16);


    byte3 = temp & 0xff00 ;
    byte3 = byte3 >> (8);


    byte4 = temp & 0xff ;

    if(putchar(byte1)==EOF){
        return 0;
    }
    //debug("%x",byte1);
    //debug("Added first byte of sample");

    if(putchar(byte2)==EOF){
        return 0;
    }
    //debug("%x",byte2);
    //debug("Added second byte of sample");

    if(putchar(byte3)==EOF){
        return 0;
    }
    //debug("%x",byte3);
    //debug("Added third byte of sample");

    if(putchar(byte4)==EOF){
        return 0;
    }
    //debug("%x",byte4);
    //debug("Added fourth byte of sample");

    temp = hp->channels;

    byte1 = temp & 0xff000000 ;
    byte1 = byte1 >> (24);


    byte2 = temp & 0xff0000 ;
    byte2 = byte2 >> (16);


    byte3 = temp & 0xff00 ;
    byte3 = byte3 >> (8);


    byte4 = temp & 0xff ;

    if(putchar(byte1)==EOF){
        return 0;
    }
    //debug("%x",byte1);
    //debug("Added first byte of channel");

    if(putchar(byte2)==EOF){
        return 0;
    }
    //debug("%x",byte2);
    //debug("Added second byte of channel");

    if(putchar(byte3)==EOF){
        return 0;
    }
    //debug("%x",byte3);
    //debug("Added third byte of channel");

    if(putchar(byte4)==EOF){
        return 0;
    }
    //debug("%x",byte4);
    //debug("Added fourth byte of channel");

    return 1;
}



int read_annotation(char *ap, unsigned int size){
    unsigned int byte;
    for(int i =0;i<size;i++){
        byte = getchar();
        *(ap+i) = byte;
    }
    if (byte != '\0'){
        return 0;
    }
    //debug("read annotation");
    return 1;
}


int write_annotation(char *ap, unsigned int size){
    for (int i=0;i<size;i++){
        if(putchar(*(ap+i))==EOF){
            return 0;
        }
    }
    //debug("write annotation");
    return 1;
}


int read_frame(int *fp, int channels, int bytes_per_sample){
    for(int i =0;i<channels;i++){
        int getCharNumber=0;
        int storedNumber=0;
        int mostSignificantBit;
        *(fp+i) = 0; // Sets the buffer to 0;
        for(int j =0; j<bytes_per_sample; j++){
            if((getCharNumber = getchar())==EOF){
                return 0;
            }

         storedNumber = getCharNumber ^ storedNumber;
            if(j != bytes_per_sample-1){
                storedNumber  = storedNumber << (8);
            }

        }

        if (bytes_per_sample == 1){
            mostSignificantBit = storedNumber & 0x80;
            if(mostSignificantBit)
            storedNumber = storedNumber ^ 0xffffff00;
        }
        if (bytes_per_sample == 2){
            mostSignificantBit = storedNumber  & 0x8000;
            if(mostSignificantBit)
            storedNumber = storedNumber ^ 0xffff0000;
        }
        if (bytes_per_sample == 3){
            mostSignificantBit = storedNumber & 0x800000;
            if(mostSignificantBit)
            storedNumber = storedNumber ^ 0xff000000;
        }
        //debug("%d",storedNumber);
        *(fp+i) = storedNumber;
    }

return 1;
}


int write_frame(int *fp, int channels, int bytes_per_sample){
    unsigned int shiftAmount;
    unsigned int value;
    for (int i=0;i<channels;i++){
        for(int j=bytes_per_sample;j>0;j--){
            value = *(fp+i);
            shiftAmount = 0xff << (j-1)*8;
            value = value & shiftAmount;
            value = value  >> (j-1)*8;
            if(putchar(value)==EOF)
                return 0;
            }
        }

    return 1;
}



int compareStrings(char* input,char* flag){
    int offset=0;
    while(*(input+offset) != '\0'){
        if (*(input+offset) != *(flag+offset)){
            return 0;
        }
        offset++;
    }
    if (*input+offset != *flag+offset){
        return 0;
    }
     return 1;
}


int returnFailure(){
    global_options=0;
    return 0;
}

int myAtoi(char * x){
    int total = 0;
    int i = 0;
    while(*(x+i) && (*(x+i)>= '0' && *(x+i)<='9')){
        total=total*10 + (*(x+i)-'0');
        i++;
    }
    return total;
}


unsigned long myStroul(char* x){
    int offset=0;
    unsigned long number=0;
    while((*(x+offset))!='\0'){
        number = number << (4);
        char currentChar = *(x+offset);
        if(currentChar >= 0x41 && currentChar <= 0x46){ // Capital
            currentChar = currentChar - 'A';
            currentChar = currentChar + 10;
            number = number ^ currentChar;
            offset++;
        }
        if(currentChar >= 0x61 && currentChar <=0x66){ // Lowercase
            currentChar = currentChar - 'a';
            currentChar = currentChar + 10;
            number = number ^ currentChar;
            offset++;
        }
        if(currentChar >= 0x30 && currentChar <=0x39){ // Number
            currentChar = currentChar - '0';
            currentChar = currentChar + 0;
            number = number ^ currentChar;
            offset++;
        }
    }
    return number;

}



