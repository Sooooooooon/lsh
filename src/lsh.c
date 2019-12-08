/***************************************************************************//**

  @file         lsh.c

  @author       Kim soon, Park yang soo, Lim seong mook

  @date         Sunday,  8 December 2019

  @brief        LSH (Libstephen SHell)

*******************************************************************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <termios.h>


#define FILE_PATH_MAX_LENGTH 	100
#define FILE_MAX_COUNT  		200
#define PATH_MAX_LENGTH			255
#define FILE_MAX_LENGTH 		20
#define EXT_MAX_LENGTH  		5
#define	ONE_LINE_FILE_NUMBER	4

#define FALSE     				0
#define TRUE     				1

#define ATTRIBUTE_OFF			0
#define BOLD					1

#define BLACK					30
#define RED						31
#define	GREEN					32
#define YELLOW					33
#define BLUE					34
#define MAGENTA					35
#define CYAN					36
#define	WHITE					37

/*
  Function Declarations for builtin shell commands:
 */

 struct FileEntry
{
	int  nNumber;
	char szName[FILE_MAX_LENGTH];
	int  nFileType;
};
int getch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}
void GetFileType(char* szFileName, int* nFileType)
{
	struct stat fileStatus;
	lstat(szFileName, &fileStatus);

	if (S_ISBLK(fileStatus.st_mode)) 		*nFileType = BLOCK_SPECIAL_FILE;
	else if (S_ISCHR(fileStatus.st_mode))	*nFileType = CHAR_SPECIAL_FILE;
	else if (S_ISDIR(fileStatus.st_mode))	*nFileType = DIRECTORY;
	else if (S_ISFIFO(fileStatus.st_mode))	*nFileType = PIPE_FILE;
	else if (S_ISLNK(fileStatus.st_mode))	*nFileType = LINK_FILE;
	else if (S_ISREG(fileStatus.st_mode))	*nFileType = REG_FILE;
	else if (S_ISSOCK(fileStatus.st_mode))	*nFileType = SOCKET_FILE;
}
void GetCurrentDirectoryFiles(int* pFileEntryCount, struct FileEntry* arrFileEntry)
{
	// Init
	*pFileEntryCount = 0;

	// Read Directory
	struct dirent* pDirent;
	DIR* currentDirectory = opendir("./");

	while((pDirent = readdir(currentDirectory)) != NULL)
	{
		// Input File Number
		arrFileEntry[*pFileEntryCount].nNumber = *pFileEntryCount;

		// Input File Name
		strncpy(arrFileEntry[*pFileEntryCount].szName, pDirent->d_name, FILE_MAX_LENGTH);
		int i;
		for(i = strlen(pDirent->d_name); i < FILE_MAX_LENGTH; i++)
		{
			arrFileEntry[*pFileEntryCount].szName[i] = ' ';
		}
		arrFileEntry[*pFileEntryCount].szName[FILE_MAX_LENGTH-1] = 0;

		// Input File Kind
		GetFileType(pDirent->d_name, &arrFileEntry[*pFileEntryCount].nFileType);

		// FIle Count Increase
		(*pFileEntryCount)++;
	}

	closedir(currentDirectory);
}

void PrintSystemTime()
{
	time_t t;
	time(&t);

	printf("%s", ctime(&t));
}


int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_team12(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "team12"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}
/**
  @ ADD fuction team12
*/
int lsh_team12(char  **args)
{
	int					nIsCopy	= TRUE;
	int					nCursorIndex = 0;
	int 				nFileEntryCount = 0;
	struct FileEntry 	arrFileEntry[FILE_MAX_COUNT];
	char 				szOriginalFilePath[FILE_PATH_MAX_LENGTH];
	char 				szDestinationFileName[FILE_MAX_LENGTH];


	while(TRUE)
	{
		system("clear");

		int i = 0;
		char szCurrentPath[PATH_MAX_LENGTH];
		char szRealName[FILE_MAX_LENGTH];

		// Status Bar Display
		getcwd(szCurrentPath, PATH_MAX_LENGTH);
		printf("%c[%d;%d;%dm  Path : %s  |  File Count : %d  |  ", 27, BOLD, BLACK, BACKGROUND_WHITE, szCurrentPath, nFileEntryCount);
		PrintSystemTime();

		// File Display
		for (i = 0; i < nFileEntryCount; i++)
		{
			if(i % ONE_LINE_FILE_NUMBER == 0)
			{
				printf("\n");
			}

			printf("%c[%d;%d;%dm%20s\t", 27, ATTRIBUTE_OFF, RED + arrFileEntry[i].nFileType, (nCursorIndex == i) ? BACKGROUND_RED : BACKGROUND_BLACK, arrFileEntry[i].szName);
		}

		printf("\n");

		// Real File Name
		strncpy(szRealName, arrFileEntry[nCursorIndex].szName, FILE_MAX_LENGTH);
		for (i = 0; i < FILE_MAX_LENGTH; i++)
		{
			if (szRealName[i] == ' ')
			{
				szRealName[i] = 0;
				break;
			}
		}

		// Cursor Move
		char cInput = getch();
		if (cInput == 'd')
		{
			nCursorIndex++;
		}
		else if (cInput == 'a')
		{
			nCursorIndex--;
		}
		else if (cInput == 'w')
		{
			nCursorIndex -= ONE_LINE_FILE_NUMBER;
		}
		else if (cInput == 's')
		{
			nCursorIndex += ONE_LINE_FILE_NUMBER;
		}
		// execute
		else if (cInput == 'e')
		{
			if (arrFileEntry[nCursorIndex].nFileType == DIRECTORY)
			{
				// Current Directory Change
				chdir(szRealName);

				// Reload
				GetCurrentDirectoryFiles(&nFileEntryCount, arrFileEntry);
				nCursorIndex = 0;
			}
			else
			{
				ExecuteProgram(arrFileEntry[nCursorIndex].szName);
			}
		}
		// quit
		else if (cInput == 'q')
		{
			break;
		}
		// delete
		else if (cInput == 'z')
		{
			if (arrFileEntry[nCursorIndex].nFileType == DIRECTORY)
			{
				rmdir(szRealName);
			}
			else
			{
				unlink(szRealName);
			}

			// Reload
			GetCurrentDirectoryFiles(&nFileEntryCount, arrFileEntry);
			nCursorIndex = 0;
		}
		// Create Directory
		else if (cInput == 'n')
		{
			mkdir("new_directory", 0755);

			// Reload
			GetCurrentDirectoryFiles(&nFileEntryCount, arrFileEntry);
			nCursorIndex = 0;
		}


	}

	return 1;
}


/**

   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code

 */

int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
