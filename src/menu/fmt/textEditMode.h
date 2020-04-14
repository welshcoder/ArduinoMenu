/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, print edit mode cursor for fields
*/

namespace Menu {
  template<typename O>
  struct TextEditModeFmt:public O {
    using This=TextEditModeFmt<O>;
    template<bool io,bool toPrint=true>
    inline void fmtMode(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(io) switch(m) {
        case Mode::Normal: O::raw(' ');break;
        case Mode::Edit: O::raw(s?':':' ');break;
        case Mode::Tune: O::raw(s?'>':' ');break;
      }
      O::template fmtMode<io,toPrint>(n,s,e,m);
    }
  };
};