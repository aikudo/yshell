// $Id: inode.h,v 1.1 2014-05-25 12:19:49-07 - - $

#ifndef __INODE_H__
#define __INODE_H__

#include <exception>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

#include "util.h"

//
// inode_t -
//    An inode is either a directory or a plain file.
//

enum inode_t {DIR_INODE, FILE_INODE};

//
// directory -
//    A directory is a list of paired strings (filenames) and inodes.
//    An inode in a directory may be a directory or a file.
//

class inode; //class body can be defined later
typedef map<string, inode*> directory;
//inode uses directory typedef 

//
// inode_state -
//    A small convenient class to maintain the state of the simulated
//    process:  the root (/), the current directory (.), and the
//    prompt.
//

class inode_state {
   friend class inode;
   friend ostream& operator<< (ostream& out, const inode_state&);
   private:
      inode_state (const inode_state&) = delete; // copy ctor
      inode_state& operator= (const inode_state&) = delete; // op=
      inode* root {nullptr};
      inode* cwd {nullptr};
      string prompt {"% "};
      vector <string> istr; //map of inode# -> names
   public:
      inode_state();  // what is this? not a ctor?
      void myprompt(const string& prompt_) { prompt = prompt_; } 
      string myprompt() const { return prompt; };
      inode* iroot(void){ return root; };
      inode* icwd(void){ return cwd; };
      void icwd(inode* cwd){ this->cwd = cwd; };
      const string& inodestr(int inr){ return istr[inr]; };
      void inodestr(int inr, string name){ istr[inr] = name; };
};

//need to fill this out to print the prompt?
ostream& operator<< (ostream& out, const inode_state&);


//
// class inode -
//
// inode ctor -
//    Create a new inode of the given type, using a union.
// get_inode_nr -
//    Retrieves the serial number of the inode.  Inode numbers are
//    allocated in sequence by small integer.
// size -
//    Returns the size of an inode.  For a directory, this is the
//    number of dirents.  For a text file, the number of characters
//    when printed (the sum of the lengths of each word, plus the
//    number of words.
// readfile -
//    Returns a copy of the contents of the wordvec in the file.
//    Throws an yshell_exn for a directory.
// writefile -
//    Replaces the contents of a file with new contents.
//    Throws an yshell_exn for a directory.
// remove -
//    Removes the file or subdirectory from the current inode.
//    Throws an yshell_exn if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).
// mkdir -
//    Creates a new directory under the current directory and 
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.
// mkfile -
//    Create a new empty text file with the given name.  Error if
//    a dirent with that name exists.
//    


class inode {
   friend class inode_state;
   private:
      int inode_nr;
      inode_t type;
      //string & dirname ;
      union {
         directory* dirents;
         wordvec* data;
      } contents;
      static int next_inode_nr;
   public:
      inode (inode_t init_type);   //default ctor
      inode (const inode& source); // copy ctor
      inode& operator= (const inode& from); // copy = 
      ~inode (); // dtor
      int get_inode_nr() const;
      int size() const;
      const wordvec& readfile() const;
      void writefile (const wordvec& newdata);
      void remove (const string& filename);
      inode& mkdir (const string& dirname);
      inode* dir(const string& dirname); //ret name-> inode 
      string dir(const inode *inode) const; //ret inode-> string
      inode& mkfile (const string& filename);
      const inode& getfile (const string& filename);
      inode* parent() ;
      const directory& getdir() { return *contents.dirents; } ;
      //const string & getname();

};


#endif

