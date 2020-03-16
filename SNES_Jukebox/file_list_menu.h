#ifndef FILE_LIST_MENU_H
#define FILE_LIST_MENU_H

struct FileListMenu {
public:

  FileListMenu(byte filesPerPage) {
    this->filesPerPage = filesPerPage;
  }
  
  void initialize(File &root) {
    if (this->root) {
      this->root.close();
    }
    this->root = root;
    currentPage = 0;
    currentFile = 0;
    
    countFiles();
  }
  
  void countFiles() {
    pageCount = 1;
    totalFileCount = 0;
    
    File f = root.openNextFile();
    while (f) {
      f.close();
      
      totalFileCount++;
      if (totalFileCount % filesPerPage == 0) {
        pageCount++;
      }
      
      f = root.openNextFile();
    }
    root.rewindDirectory();
  }
  
  bool readNextFile(char *filename, bool &isDirectory) {
    if (currentFile == getCurrentPageSize()) return false;
    
    File f = root.openNextFile();
    if (!f) {
      return false;
    } else {
      memcpy(filename, f.name(), 12);
      filename[12] = 0;
      isDirectory = f.isDirectory();
      f.close();
    }
    
    currentFile++;
    
    return true;
  }
  
  void resetPage() {
    if (currentFile == 0) {
      return;
    }
    
    root.rewindDirectory();
    for (int i = 0; i < currentPage; i++) {
      for (int j = 0; j < filesPerPage; j++) {
        File f = root.openNextFile();
        f.close();
      }
    }
    currentFile = 0;
  }

  bool previousPage() {
    if (currentPage == 0) {
      return false;
    }
    
    currentPage--;
    resetPage();

    return true;
  }
  
  bool nextPage() {
    if (currentPage + 1 == pageCount) {
      return false;
    }
    
    char temp[13];
    bool temp2;
    while (readNextFile(temp, temp2)) {
      // Call readNextFile() until at end of page
    }
    
    currentPage++;
    currentFile = 0;
    
    return true;
  }

  File getSelectedFile(byte selectedIndex) {
    resetPage();
    char temp[13];
    bool temp2;
    for (byte b = 0; b < selectedIndex; b++) {
      readNextFile(temp, temp2);
    }
    File result;
    result = root.openNextFile();
    currentFile++;
    return result;
  }
  
  byte getCurrentPageSize() {
    if (currentPage + 1 < pageCount) {
      return filesPerPage;
    }
    return totalFileCount % filesPerPage;
  }

private:
  byte filesPerPage;

  File root;
  
  word currentPage;
  byte currentFile;
  
  word pageCount;
  word totalFileCount;
};

#endif

