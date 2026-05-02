#pragma once
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP8266)

  #include <FTPClient_Generic.h>
  using FTPClientType = FTPClient_Generic;

  inline void ftp_open      (FTPClientType& ftp)                              { ftp.OpenConnection(); }
  inline void ftp_close     (FTPClientType& ftp)                              { ftp.CloseConnection(); }
  inline void ftp_chdir     (FTPClientType& ftp, const char* dir)             { ftp.ChangeWorkDir((char*)dir); }
  inline void ftp_init_binary(FTPClientType& ftp)                             { ftp.InitFile("Type I"); }
  inline void ftp_newfile   (FTPClientType& ftp, const char* name)            { ftp.NewFile((char*)name); }
  // buffer NO-const (la lib lo pide así)
  inline void ftp_write     (FTPClientType& ftp, uint8_t* d, size_t n)        { ftp.WriteData(d, n); }
  inline void ftp_closefile (FTPClientType& ftp)                              { ftp.CloseFile(); }

#else  // ===== ESP32 =====

  #include <ESP32_FTPClient.h>
  using FTPClientType = ESP32_FTPClient;

  inline void ftp_open      (FTPClientType& ftp)                              { ftp.OpenConnection(); }
  inline void ftp_close     (FTPClientType& ftp)                              { ftp.CloseConnection(); }
  inline void ftp_chdir     (FTPClientType& ftp, const char* dir)             { ftp.ChangeWorkDir(dir); }
  inline void ftp_init_binary(FTPClientType& ftp)                             { ftp.InitFile("Type I"); }
  inline void ftp_newfile   (FTPClientType& ftp, const char* name)            { ftp.NewFile(name); }
  // firma de ESP32_FTPClient: WriteData(unsigned char*, int)
  inline void ftp_write     (FTPClientType& ftp, uint8_t* d, size_t n)        { ftp.WriteData((unsigned char*)d, (int)n); }
  inline void ftp_closefile (FTPClientType& ftp)                              { ftp.CloseFile(); }

#endif
