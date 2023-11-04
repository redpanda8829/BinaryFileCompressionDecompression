#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Stores parameters that specify how to the program should behave.
 *
 * Populated by the get_parms() function.  You don't really need to
 * worry about this bit.  I wrote it for you. */
struct parms {
  char *filename;
  unsigned char mode;
};

typedef struct parms Parms;



/** U S E R   I N T E R F A C E ****************************************/


/* This function simply displays helpful usage information if the
 * program is called improperly or with no command line arguments.
 *
 * You don't need to know how this function works. */
void print_usage(const char *cmd)
{
  printf("Usage: %s MODE filename\n\n"
         "Available Modes:\n"
         "  -c     Compress: Performs RLE compression on \"filename\"\n"
         "                   and writes result to \"filename.rle\"\n\n"
         "  -x     Expand: Performs RLE expansion on \"filename\". The\n"
         "                 supplied \"filename\" must have the extension\n"
         "                 \".rle\" The result is written to \"filename\"\n"
         "                 with the extension \".rle\" removed.\n\n"
         "  -d     Debug: Prints a hexdump of \"filename\" to the screen.\n\n"
         "  -g     Generate: Writes the test file described in the Project 2\n"
         "                   assignment document to disk as \"filename\". Use\n"
         "                   this file to test and debug your program.\n\n"
         "Examples:\n"
         "  %s -c test.bin\n\tProduces RLE encoded file test.bin.rle\n"
         "  %s -x test.bin.rle\n\tExpands test.bin.rle to disk as test.bin\n"
         "  %s -d test.bin.rle\n\tDisplays raw contents of test.bin.rle\n"
         "  %s -g test.bin\n\tGenerates test file with known contents to disk as test.bin\n",
         cmd, cmd, cmd, cmd, cmd);
}


/* This function parses the command line arguments supplied in
 * argc/argv and populates the Params struct with the mode and filename
 * specified by the user on the command line.
 *
 * You don't need to know how this function works. */
int get_parms(Parms *parms, const char *modes, int argc, char **argv)
{
  int i = 0;

  if (argc != 3 || *argv[1] != '-')
    return 0;

  while (modes[i] && modes[i] != *(argv[1] + 1))
    i++;

  if ( (parms->mode = i) == strlen(modes) ) {
    fprintf(stderr, "Invalid Mode %s\n", argv[1]);
    return 0;
  }

  parms->filename = argv[2];

  return 1;
}



/** H E L P E R   F U N C T I O N S ************************************/


/* Returns a newly allocated string on the heap containing the supplied
 * filename with the specified extension added to its end.  This
 * function effectively just concatenates two strings. */
char *filename_add_ext(const char *filename, const char *ext)
{
  /* using malloc to create a container big enought to store the filename with the extension that is to be added and initializing a char pointer to keep its address*/
  char *c = malloc(strlen(filename)+strlen(ext)+1);
  strcpy(c,filename);
  strcat(c,ext);
  return c;
}


/* Returns a newly allocated string on the heap containing the supplied
 * filename with its extension removed.
 *
 * For example:
 *   if `filename` contains the string "test.txt.rle", then this
 *   function will return a string on the heap containing
 *   "test.txt" */
char *filename_rm_ext(const char *filename)
{
  /* Using malloc to create a container just big enough to store the file name when the extension .rle has been removed and then
using strncpy to copy filename(without the extension) from source  to the destination filename_without_rle*/
  char *filename_wthout_rle = malloc(strlen(filename)-4);
  strncpy(filename_wthout_rle, filename, strlen(filename)-4);
  return filename_wthout_rle;
}


/* This function returns zero if the supplied filename does not have the
 * extension ".rle"; otherwise it returns a non-zero value. */
int check_ext(const char *filename)
{
  /* using the library function strcmp to compare the last four positions of file name (which would be the ext) with the string ".rle" */
  if(strcmp(filename+strlen(filename)-4,".rle")==0){
    return 1971;
  }
  else
    return 0;
}


/* This function returns zero if the supplied file does not have the
 * !RLE "magic" byte sequence as its first four bytes; otherwise it
 * returns a non-zero value. */
int check_magic(FILE *fp)
{
  /* Your code goes here! */
  char *magic = malloc(4);
  fread(magic,1,4,fp);
  if(strcmp(magic,"!RLE")==0){
    return 1969;
  }
  else
    return 0;
}



/** M O D E S **********************************************************/


/* This function performs the run length encoding (RLE) algorithm on the
 * bytes of the specified file (which must not be modified).  The result
 * is written to a new file having the supplied filename PLUS the ".rle"
 * extension.  Called when -c is supplied for mode on the command line.
 *
 *   For example, if the supplied filename is "test.bin", this function
 *   should create the file "test.bin.rle", whose contents will be the
 *   run length encoded bytes of "test.bin".
 *
 * This function is also responsible for writing the !RLE "magic" byte
 * sequence to the first four bytes of the output file, thus marking it
 * as a genuine RLE encoded file. */
void compress(const char *filename)
{
  /* Your code goes here! */
  FILE *fp;
  FILE *output_file;
  char *cmprsd_file;
  int count;
  int data;
  int data_next;
  long int posn;
  
  cmprsd_file = filename_add_ext(filename, ".rle"); /* Creating the name of the compressed file using the function filename_add_ext*/
  fp = fopen(filename,"r");
  if(fp!=NULL){ /*Checking if the passed file is valid*/
    
  output_file = fopen(cmprsd_file, "w");
  fputs("!RLE", output_file); /*Adding the magic byte sequence to the beginning of the new file to create the ./rle */
    
  while((data=getc(fp))!= EOF) /*We iterate through the whole binary file before we reach the end of file*/
  {
    count = 0;
    posn = ftell(fp); /*Saving the position of the stream*/
    data_next = fgetc(fp); 
    
      if (data==data_next) /*Checking if adjacent elements are repeating*/
      {
        count++;
          while(data==data_next) /*Checking if adjacent elements are repeating more than once*/
          {
            
          if(count!=255) /*Checking for max capacity of two digit hexadecimal*/
          {
            count++; /*Incrementing as long as count has not reached 255*/
            data_next=fgetc(fp); /*checking the next data element and shifting the stream*/
          }
          else{
            break;
          }
          }
        fseek(fp,posn+count-1,SEEK_SET); /*Shifting stream to the data element to be read next*/
        fputc(count,output_file); /*Putting the int count into the compressed file*/
        fprintf(output_file,"%c",data); /*Putting the data element into the compressed file following the number of times it has been repeated*/
      }
      else if(data!=data_next) /* If adjacent data elements do not repeat*/
      {
        count++;
        fputc(count,output_file);
        fprintf(output_file,"%c",data);
        fseek(fp,posn,SEEK_SET);
      } 
    }
    fclose(output_file);
    fclose(fp);
    }
      else{
        fprintf(stderr,"Error");
      }
    free(cmprsd_file);
  }

/* This function performs run length decoding on the bytes of the
 * supplied filename and writes the result to disk as the contents of a
 * newly created file with the supplied filename MINUS the ".rle"
 * extension.  Called when -x is supplied for mode on the command line.
 *
 *   For example, if the supplied filename is "test.bin.rle", this
 *   function will create the file "test.bin", whose contents will be
 *   the run length decoded bytes of "test.bin.rle".
 *
 * This function also checks that the supplied filename has both the
 * ".rle" extension and !RLE "magic" byte sequence.  If either is not
 * present, this function prints an appropriate error message to stderr,
 * does not create a new file, and simply returns without performing
 * any run length decoding. */
void expand(const char *filename)
{
  FILE *cp_file;
  FILE *dp_file;
  char *dp_file_name;
  int count;
  int data;
  int i;
  int magic;
  int ext_check;
  ext_check = check_ext(filename);
  dp_file_name = filename_rm_ext(filename); /*Creating the name of the decompressed file using the function filename_rm_ext*/
  cp_file = fopen(filename,"r"); /*Opening the compressed file*/
  if(cp_file==NULL){
    fprintf(stderr,"File pointer is not valid");
  }
  else{
  magic = check_magic(cp_file);
  if (magic == 0 && ext_check ==0){ /*Checking for the magic sequence and file extension*/
fprintf(stderr,"Error: Either magic sequence was not found or file did not have appropriate extension. Contact a Chad Coder"); 
  }
  else{
    dp_file = fopen(dp_file_name,"w"); /*Creating the decompressed file*/
    while((data=getc(cp_file))!= EOF) /*Iterating through the compressed file*/
    {
      count = data; /*Count is the first digit of the pair*/
      data = getc(cp_file); /*Data is the second digit of the pair*/
      for(i=0;i<count;i++) /*Writing the data element to the decompressed file as many times as the count*/
      {
        fprintf(dp_file,"%c",data);
      }
    }
    fclose(cp_file);
    fclose(dp_file);
  }
    }
   free(dp_file_name);
    /*
    free(magic);
    free(dp_file_name);*/
}


/* This function prints a raw byte dump of the specified file to the
 * screen.  Called when -d is supplied for mode on the command line.
 * This allows you to look at the raw bytes of any file you want, which
 * should very much help you debug your program.
 *
 * You don't need to know how this function works. */
void debug(const char *filename)
{
  unsigned char b[16];
  unsigned int nbytes, i;

  unsigned int offset = 0;
  FILE *fp = fopen(filename, "rb");

  if (!fp) {
    fprintf(stderr, "error -- failed to open file '%s'\n", filename);
    return;
  }

  while ( (nbytes = fread(b, sizeof(*b), 16, fp)) > 0 ) {
    printf("%08x: ", offset);
    for (i=0; i<16; i++) {
      i   < nbytes ? printf("%02x", b[i])  : printf("  ");
      ++i < nbytes ? printf("%02x ", b[i]) : printf("   ");
    }

    printf(" ");

    for (i=0; i<nbytes; i++)
      printf("%c", isprint(b[i]) ? b[i] : '.');

    printf("\n");
    offset += 16;
  }

  fclose(fp);
}


/* This function generates the test file discussed in the Project
 * description document.  Called when -g is supplied for mode on the
 * command line.  This provides a file you understand the contents of,
 * which should very much help you debug your program.
 *
 * You don't need to know how this function works. */
void generate(const char *filename)
{
  FILE *fp = fopen(filename, "wb");
  unsigned char test[] =
    "\xee\x5d\x41\x41\x41\x8e\x2a\x3a\x3a\x3a\x3a\xe8\xbd"
    "\x22\x88\x0a\xac\xac\xac\x9a\x51\x3a\x3a\x3a\xaa\xaa"
    "\xaa\x31\x61\x27\xa7\x7e\x6d\x52\xd8\x74\x6a\x6f\x6e"
    "\x4e\xe8\x90\xa0\x7d\x88\x64\x12\x21\xdb\xae\xf7\xf7"
    "\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7"
    "\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7"
    "\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7\xf7"
    "\xf7\xf7\xf7\xf7\xf7\xaa\xaa\xbb\xbb";

  if (!fp) {
    fprintf(stderr, "error -- failed to open '%s'\n", filename);
    return;
  }

  if (1 != fwrite(test, sizeof(test) - 1, 1, fp))
    fprintf(stderr, "failed to write '%s' (oh no!)\n", filename);

  fclose(fp);
}



/** M A I N ************************************************************/


int main(int argc, char **argv)
{
  Parms parms;

  /* simply an array of function pointers */
  void (*action[])(const char *filename) = {
    compress,
    expand,
    debug,
    generate
  };

  /* get mode and filename from command line arguments;
   * prints the usage and exits if improper arguments are supplied */
  if(!get_parms(&parms, "cxdg", argc, argv)) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  /* call the function for the specified mode and pass
   * it the filename supplied at the command line */
  action[parms.mode](parms.filename);

  return 0;
}