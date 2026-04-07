// IR codes for MAG LG TCL SENSOR BT_PURE_ACOUSTIC remote
//work with protocole NEC 32 bits
#ifndef CODES_REMOTE
#define CODES_REMOTES
typedef struct {
  uint32_t POWER    = 0xF708FB04;
  uint32_t SOURCE   = 0xF40BFB04;
  uint32_t Mute     = 0xF609FB04;
  uint32_t UP       = 0xBF40FB04;
  uint32_t DOWN     = 0xBE41FB04;
  uint32_t OK       = 0xBB44FB04;
  uint32_t VOL_UP   = 0xFF00FB04;
  uint32_t VOL_DOWN = 0xFE01FB04;
  uint32_t MENU     = 0x837CFB04;
  uint32_t LEFT     = 0xF807FB04;
  uint32_t RIGHT    = 0xF906FB04;
}LG_CODE_IR;


typedef struct {
  uint32_t POWER    = 0xF708FB04;
  uint32_t SOURCE   = 0xF40BFB04;
  uint32_t Mute     = 0xF609FB04;
  uint32_t UP       = 0xBF40FB04;
  uint32_t DOWN     = 0xBE41FB04;
  uint32_t OK       = 0xBB44FB04;
  uint32_t VOL_UP   = 0xFD02FB04;
  uint32_t VOL_DOWN = 0xFC03FB04;
  uint32_t HOME     = 0x837CFB04;
  uint32_t LEFT     = 0xF807FB04;
  uint32_t RIGHT    = 0xF906FB04;
}MAG_CODE_IR;



typedef struct {
  uint32_t POWER    =  0xAD52DF20;
  uint32_t SOURCE   =  0xBF40DF20;
  uint32_t Mute     =  0xAC53DF20;
  uint32_t UP       =  0xB847DF20;
  uint32_t DOWN     =  0xB24DDF20;
  uint32_t OK       =  0xB54ADF20;
  uint32_t VOL_UP   =  0xFC03DF20;
  uint32_t VOL_DOWN =  0xBE41DF20;
  uint32_t HOME     =  0xF906DF20;
  uint32_t LEFT     =  0xB649DF20;
  uint32_t RIGHT    =  0xB44BDF20;
  }SENSOR_CODE_IR;

typedef struct {
  uint32_t POWER    =    0xAB054F;
  uint32_t SOURCE   =    0x3A0C5F;
  uint32_t Mute     =    0x30FCF;
  uint32_t UP       =    0x6509AF;
  uint32_t DOWN     =    0xE501AF;
  uint32_t OK       =    0xD002FF;
  uint32_t VOL_UP   =    0xB0F4F;
  uint32_t VOL_DOWN =    0x8B074F;
  uint32_t HOME     =    0xEF010F;
  uint32_t LEFT     =    0x9506AF;
  uint32_t RIGHT    =    0x150EAF;
  uint32_t Back     =    0x1B0E4F;
  //uint32_t BUTTON_123 =  0x4C0B3F;
  }TCL_CODE_IR;

  typedef struct {
    
   uint32_t POWER  =       0xF30C0E0E;
   uint32_t SOURCE  =       0xF00F0E0E;
   uint32_t Mute   =       0xF20D0E0E;
   uint32_t UP     =       0xBD420E0E;
   uint32_t DOWN   =       0xBC430E0E;
   uint32_t OK     =       0xB9460E0E;
   uint32_t VOL_UP =       0xEB140E0E;
   uint32_t VOL_DOWN =     0xEA150E0E;
   uint32_t HOME     =     0x87780E0E;
   uint32_t LEFT     =     0xBB440E0E;
   uint32_t RIGHT    =     0xBA450E0E;

  }SKYWORTH_CODE_IR;


  typedef struct {
  uint32_t POWER      =  0x78871608;
  uint32_t Mute       =  0x23DC1608;
  uint32_t UP         =  0x7B841608;
  uint32_t DOWN       =  0x728D1608;
  uint32_t PLAY_PAUSE =  0x4DB21608;
  uint32_t MODE       =  0x758A1608;
  uint32_t PREVIOUS   =  0x44BB1608;
  uint32_t NEXT       =  0x718E1608;
  uint32_t BT         =  0x4AB51608;
  uint32_t _3D        =  0x37C81608;
  uint32_t LINK       =  0x76891608;
}PURE_ACOUSTICS_BT_CODE_IR;

#endif