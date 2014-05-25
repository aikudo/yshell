// $Id: commands.cpp,v 1.1 2014-05-25 12:19:49-07 - - $

#include "commands.h"
#include "debug.h"
#include <stack>
//#include <algorithm>
commands::commands(): map ({
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
}){}

function commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (function)
   commandmap::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}


void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
//   assert(words.size() >2);
   inode* cwd = state.icwd();
   const inode& file = cwd->getfile(words[1]);
   const wordvec &data = file.readfile();
   for (auto i : data)  cout << " " << i << endl;
}

//change only one directory at a time.
//arg is either "/" or "pathname" e.g. not/like/this/that
//words[0] = "cd", words[1] = "pathname" ....
void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode* cwd = state.icwd();

   if (words.size() > 2){
      throw yshell_exn ( "too many args:" );
   }else if( words.size() == 1 || words[1].compare("/") == 0){
      state.icwd(state.iroot()); //go to root
   }else if( words[1].compare(".") == 0){
      //do nothing
   }else if (words[1].compare("..") == 0){
      state.icwd(state.icwd()->parent());
   }else { //lookup for dir entry
      state.icwd(cwd->dir(words[1]));
   }
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   for(auto i = next(words.cbegin()); i != words.cend(); ++i)
      cout << *i << " "; //extra space on last word
   cout << endl;
}

// non-numberic arg -> exit with 127
void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   int exit_code = 0;
   if (words.size() > 1){
      string::size_type sz = 0;
      try{
         exit_code = stoi(words[1], &sz); //parse str->int
      }catch( const exception& e ){
         //cout << __LINE__ << ": code(" << words[1] + ")" << endl;
         exit_code = 127;
      }
      if (sz < words[1].length() ) exit_code = 127;
   }
   cout<<"exit code: " << exit_code << endl;
   exit(exit_code);
   //throw ysh_exit_exn( exit_code );
   //how to return exit code?
   //(void) exit_code;
   throw ysh_exit_exn(  );
}

   //<6SPACES><INODE><2SPACES><6spaces><SIZE><2SPACES><FILENAME>
void ls_print(int inode_nr, int size, const string& name){
   //auto curwidth = cout.width();
   cout.width(6);
   cout << inode_nr << "  ";
   cout.width(6);
   cout << size << "  " << name << endl;
}

void fn_ls (inode_state& state, const wordvec& words){
   //output format
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   fn_pwd(state, words);

   //this should be inside a node? so that it can print out
   //whether it's a file or directory?

   //a vector of "string", "node"
   for(auto i: state.icwd()->getdir()){
      //cout << i.first + "  " << '[' + i.second + ']' << endl;
      ls_print(i.second->get_inode_nr(),
         i.second->size(), i.first);
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}


void fn_make (inode_state& state, const wordvec& words){
   //call mkfile
   //then call writefile
   //assert (words.size() >2);
   //make we have at least a command + filename + data
   //cout<< "make file: " << words[1] << "with " << words << endl;
   string filename = words[1];
   inode &newnode = state.icwd()->mkfile(filename);
   //state.inodestr(newnode.get_inode_nr(), filename); //save an entry
   newnode.writefile(words);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

//need to parse for /something/thislike/thisacrazy/one
//checkings...
void fn_mkdir (inode_state& state, const wordvec& words){
   string dirname = words[1];
   inode& newdir = state.icwd()->mkdir(dirname);
   (void) newdir;
   //state.inodestr(newdir.get_inode_nr(), dirname); //save an entry
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   //set prompt equal to word[0]_word[1]....word[n]_
   //for( auto str: words) cout << "params: " << str << endl;
   string prompt;
   for( unsigned i = 1 ; i < words.size(); ++i ) {
      prompt += words[i] + " ";
      //cout << "index " << i << " word: " << words[i] << endl;
   }
   //cout << "prompt output:" << prompt << endl;

   state.myprompt(prompt);
/*
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   */
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);  //---> current location
   DEBUGF ('c', words);  //---> command list string
   inode* root = state.iroot();
   inode* iter = state.icwd();
   stack <string> dirs;
   while (iter !=root){ //->>> infinite loop!
   //traverse from current to root.
      inode* child = iter;
      iter =  iter->parent();
      string dirname = iter->dir(child);
      dirs.push(dirname);
      cout<< "visited: " + dirname <<endl;
   }
   cout << "/";
   while(!dirs.empty()){
      cout << dirs.top() << "/";
      dirs.pop();
   }
   cout << ":" << endl;
}


void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

