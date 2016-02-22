#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <string>

#include "def.h"

using namespace std;

string getFileContents(const char *filename)
{
    ifstream in(filename, ios::in | ios::binary);
    if (in)
    {
      return(string((istreambuf_iterator<char>(in)), istreambuf_iterator<char>()));
    }
    throw(errno);
}

void usage(void)
{

	fprintf(stderr, "ccenv, "
		"usage:\n"
        "ccenv print\n"
		"ccenv set [variable name] [variable value]\n"
		"ccenv get [variable name]\n\n"
	);
}

char *getSetValueByUser(int argc, char *argv[]) {

    int i, len;
	char *name;
	char *value = NULL;
	char *tmpval = NULL;

	if (argc < 4) {
		return NULL;
	}

	name = argv[2];

	len = strlen(name) + 2;
	for (i = 3; i < argc; ++i)
		len += strlen(argv[i]) + 1;

	/* Allocate enough place to the data string */
	for (i = 3; i < argc; ++i) {
		char *val = argv[i];
		if (!value) {
			value = (char *)malloc(len - strlen(name));
			if (!value) {
				fprintf(stderr,
				"Cannot malloc %zu bytes: %s\n",
				len - strlen(name), strerror(errno));
				return NULL;
			}
			memset(value, 0, len - strlen(name));
			tmpval = value;
		}
		if (i != 3)
			*tmpval++ = ' ';
		while (*val != '\0')
			*tmpval++ = *val++;
	}
	
    return value;
}

int setCcenv(int argc, char *argv[]) {

    if (argc < 4) {
        return -1;
    }

    char *cSetValue = getSetValueByUser(argc, argv); 
    char *cEnvName = argv[2];

    string line;
    fstream envFileIn, envFileOut;
    envFileIn.open(ENV_FILEPATH, fstream::in);
    envFileOut.open(((string)ENV_FILEPATH + ".tmp").c_str(), fstream::out);

    //char buf[131072];
    //memset(buf, '\0', 131072);
    //char *p = buf;

    if (envFileIn.is_open() && envFileOut.is_open())
    {
        bool bIsFound = false;
        while ( getline (envFileIn, line, '\0') && !envFileIn.eof())
        {
            size_t pos = line.find(cEnvName);
            if (pos != string::npos) {
                std::size_t pos2 = line.find("=");
                if (pos2 != string::npos) {
                    //cout << line.substr(pos2+1) << '\n';
                    line.erase(pos2 + 1, line.size() - pos2 + 1);
                    line += cSetValue;
                    cout << "set: "<< line << '\n';
                    bIsFound = true;
                }
            }

            //strncpy(p, line.c_str(), line.size() + 1);
            //strcpy(p+line.size(), "\n");
            //p += line.size() + 1;

            //memcpy (p, line.c_str(), sizeof(line.c_str()+1));
            //p += line.size() + 1;

            //strcat(buf, line.c_str());

            if (line.size() == 0)
                break;

            envFileOut << line << '\0';
        }

        //envFileOut << buf;

        if (!bIsFound) 
        {
            string newline = (string)cEnvName + "=" + (string)cSetValue;
            envFileOut << newline << '\0';
            cout << "set: "<< newline << '\n';
        }

        for (; envFileOut.tellp() < 131072;) {
            envFileOut << '\0';
        }
        envFileIn.close();
        envFileOut << flush;
        envFileOut.close();

        if(remove(ENV_FILEPATH) == 0)
            rename(((string)ENV_FILEPATH + ".tmp").c_str(), ENV_FILEPATH);

        system(((string)FACTORY_EXECUTABLE_FILE + " save > /dev/null 2>&1" ).c_str());

    }
    else cout << "Unable to open file\n";

    //printf("DDD: %s\n", buf);

    if (cSetValue) 
        free(cSetValue);

    return 0;
}


char *getCcenv(char *envName)
{
    //char *ret = NULL;

    string line;
    ifstream envFile(ENV_FILEPATH);

    if (envFile.is_open())
    {
        while ( getline (envFile, line, '\0') && !envFile.eof() )
        {
            size_t pos = line.find(envName);
            if (pos != string::npos) {
                std::size_t pos2 = line.find("=");
                if (pos2 != string::npos) 
                    cout << line.substr(pos2+1) << '\n';
            }
            if (line.size() == 0) 
                break;
        }
        envFile.close();
    }
    else cout << "Unable to open file\n";
    
    return NULL; 
}


int printCcenv()
{
    string line;
    ifstream envFile(ENV_FILEPATH);

    if (envFile.is_open())
    {
        int iCurLine = 0;
        while ( getline (envFile, line, '\0') && !envFile.eof() )
        {
            iCurLine++;
            if (iCurLine == 1) {
                line.erase(0, 4);
            }
            cout << line << '\n'; 
            if (line.size() == 0) 
                break;
        }
        envFile.close();
    }
    else cout << "Unable to open file\n";
    
    return 0; 
}


bool setToDefault() {

    string line;
    fstream envFile;
    envFile.open(ENV_FILEPATH, fstream::out | fstream::trunc);

    if (envFile.is_open())
    {
        int envValusSize = sizeof(default_env)/sizeof(default_env[0]);
        for (int i = 0; i < envValusSize; i++) 
        {
            envFile << default_env[i][0] << "=" << default_env[i][1] << '\0';
        }

        for (;envFile.tellp() < 131072;) {
            envFile << '\0';
        }
        envFile << flush;
        envFile.close();

        system(((string)FACTORY_EXECUTABLE_FILE + " save > /dev/null 2>&1" ).c_str());

    }
    else cout << "Unable to open file\n";

    return true;
}

bool envInit() 
{
    struct stat buf;

    if( stat(FACTORY_EXECUTABLE_FILE, &buf) == -1 ) {
        cout << "Could not find factory execuable file: " << FACTORY_EXECUTABLE_FILE << endl;
        return false;
    }

    system(((string)"rm -rf " + ENV_DIRPATH).c_str());
    system(((string)FACTORY_EXECUTABLE_FILE + " load > /dev/null 2>&1").c_str());

    if( stat(ENV_FILEPATH, &buf) == -1 ) {
        //setToDefault();
        //system(((string)FACTORY_EXECUTABLE_FILE + " load > /dev/null 2>&1").c_str());
        cout << "Could not find env file: " << FACTORY_EXECUTABLE_FILE << endl;
        cout << "Error: Env is not in factory." << endl;
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{

#ifdef _ONDEVICE_
    if (!envInit()) {
        return 0; 
    }
#endif

    char *cmdname = argv[1];
    if (argc == 1) {
        usage();
    }
    else if (argc == 2) {
        if (strcmp(cmdname, CMD_PRINTENV) == 0) 
            printCcenv();
    } else if (argc >= 3) {
    	if (strcmp(cmdname, CMD_GETENV) == 0) {
            getCcenv(argv[2]);
    	} 

        if (argc >= 4) {
            if (strcmp(cmdname, CMD_SETENV) == 0) {
                setCcenv(argc, argv);
            } 
        }
    }

	return 0;
}

