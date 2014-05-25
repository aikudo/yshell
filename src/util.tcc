// $Id: util.tcc,v 1.1 2014-05-25 12:19:49-07 - - $

template <typename item_t>
ostream& operator<< (ostream& out, const vector<item_t>& vec) {
   bool want_space = false;
   for (typename vector<item_t>::const_iterator itor = vec.cbegin();
        itor != vec.cend(); ++itor) {
      if (want_space) out << " ";
                 else want_space = true;
      out << *itor;
   }
   return out;
}

