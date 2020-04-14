/* -*- C++ -*- */
#pragma once

#include "api.h"

namespace Menu {
  ////////////////////////////////////////////////////////////////////////////////
  // printers

  //to avoid passing the output object back and forth a printer must be a top level vomposition
  //because it needs do call back data for item printing
  struct FullPrinter {
    template<typename O>
    struct Part:O {
      using This=FullPrinter::Part<O>;
      using Base=O;
      template<typename It,typename Nav,Op op=Op::Printing>
      void printMenu(It& it,Nav& nav,bool fullPrint) {
        trace(MDO<<"FullPrinter::printMenu fullPrint:"<<fullPrint<<" Op:"<<op<<" @"<<((long)&Base::obj())<<endl);
        constexpr bool toPrint=op==Op::Printing;
        O::newView();
        O::template fmt<Tag::Menu,true,toPrint>();
        it.template printTitle<Nav,typename Base::Type,op>(nav,Base::obj(),fullPrint);
        if (O::posTop(nav)) it.changed(true);
        trace(MDO<<"FullPrinter::printMenu calling printItems"<<endl);
        it.template printItems
          <Nav,typename Base::Type,op>
          (nav,Base::obj(),fullPrint,0,O::top());
        O::template fmt<Tag::Menu,false>(fullPrint);
        if (toPrint) O::lastDrawn(&it);
      }
      template<typename It,typename Nav,Op op=Op::Printing>
      void printTitle(It& it,Nav& nav) {
        trace(MDO<<"FullPrinter::printTitle"<<endl);
        constexpr bool toPrint=op==Op::Printing;
        O::template clrLine<toPrint>(O::posY());
        O::template fmt<Tag::Item,true,toPrint>();
        O::template fmt<Tag::Title,true,toPrint>();
        it.template print<Nav,typename Base::Type,op>(nav,Base::obj());
        O::template fmt<Tag::Title,false,toPrint>();
        O::template fmt<Tag::Item,false,toPrint>();
      }
      template<typename It,typename Nav,Op op=Op::Printing>
      void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        trace(MDO<<"FullPrinter::printItem"<<endl);
        constexpr bool toPrint=op==Op::Printing;
        Base::template clrLine<toPrint>(Base::posY());
        Base::template fmt<Tag::Item,  true ,toPrint>(n,s,e,m);
        Base::template fmt<Tag::Index, true ,toPrint>(n,s,e,m);
        Base::template fmt<Tag::Index, false,toPrint>(n,s,e,m);
        Base::template fmt<Tag::Cursor,true ,toPrint>(n,s,e,m);
        Base::template fmt<Tag::Cursor,false,toPrint>(n,s,e,m);
        // it.template printItem<O::Type,Tag::Item,toPrint>(O::obj(),n,s,e,m);
        trace(MDO<<"pos:"<<Base::posX()<<","<<Base::posY()<<endl);
        it.template print<Nav,typename Base::Type,op>(nav,Base::obj());
        Base::template fmt<Tag::Item,false,toPrint>(n,s,e,m);
      }
    };
  };
}