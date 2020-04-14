/* -*- C++ -*- */
#pragma once

#include "api.h"

namespace Menu {

  struct Mutable {
    template<class I>
    struct Part:I {
      using I::I;
      using I::changed;
      inline bool changed() const {return hasChanged;}
      inline void changed(bool o) {
        trace(MDO<<"Mutable "<<(o?"dirt":"clear")<<endl);
        hasChanged=o;}
      // inline void changed(Idx,bool o) {changed(o);}

      //this can not be `protected` because of `CRTP` and `mixin` composition
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out) {
        if (op==Op::ClearChanges) {
          trace(MDO<<"Mutable::print clear "<<endl);
          changed(false);
        }// else
        I::template print<Nav,Out,op>(nav,out);
      }
    protected:
      bool hasChanged=true;
    };
  };

  /// ActionHanlder, type of action functions to associate with items
  using ActionHandler=bool (*)();

  /**
  * The Action class associates an actikon function with a menu item.
  */
  template<ActionHandler act>
  struct Action {
    template<typename I>
    struct Part:I {
      using Base=I;
      using I::I;
      using This=Action<act>::Part<I>;
      inline static ActRes activate(PathRef ref=self) {
        return ref?
          ActRes::Close:act()?
            (I::canNav()?ActRes::Open:ActRes::Stay):
            (I::canNav()?ActRes::Stay:ActRes::Close);
      }
    };
  };

  template<const char** text>
  struct StaticText {
    template<typename I=Empty<Nil>>
    struct Part:I {
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out) {
        out.template raw<decltype(text[0]),op==Op::Printing>(text[0]);
        I::template print<Nav,Out,op>(nav,out);
      }
    };
  };

  struct Text {
    template<typename I>
    struct Part:I {
      using Base=I;
      const char* text;
      inline Part(const char*o):text(o) {}
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        out.template raw<decltype(text),op==Op::Printing>(text);
        I::template print<Nav,Out,op>(nav,out);
      }
    };
  };

  //static numeric field, ranges are statically defined
  template<typename T,T& value,T low,T high,T step,T tune,bool loop=false>
  struct StaticNumField {
    template<typename I>
    struct Part:I {

      inline static bool stepVal(T delta) {
        if (delta==0) return false;
        if (delta > 0) {
          if (high-value>=delta) value+=delta;
          else value=loop?low:high;
        } else {
          if (value-low>=-delta) value+=delta;
          else value=loop?high:low;
        }
        return true;
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        out.raw(value);
        I::template print<Nav,Out,op>(nav,out);
      }

      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,PathRef ref=self) {
        trace(MDO<<"Field::cmd "<<c<<" mode:"<<nav.mode()<<endl);
        if(nav.mode()==Mode::Normal&&c!=Cmd::Enter) return I::template cmd<c,Nav>(nav,ref);
        if(ref) return false;//wtf!
        switch(c) {
          case Cmd::Enter:
            switch(nav.mode()) {
              case Mode::Normal: nav.setMode(Mode::Edit);break;
              case Mode::Edit:nav.setMode(tune==0?Mode::Normal:Mode::Tune);break;
              case Mode::Tune: nav.setMode(Mode::Normal);break;
              default: return false;
            }
            return true;
          case Cmd::Esc:
            nav.setMode(Mode::Normal);
            return false;
          case Cmd::Up: stepVal(-(nav.mode()==Mode::Edit?step:tune));break;
          case Cmd::Down: stepVal(nav.mode()==Mode::Edit?step:tune);break;
          default:return false;
        }
        return true;
      }

    protected:
      T shadow;
    };
  };

  /**
  * The Item class encapsulates a composition to be a stratic menu item.
  */
  template<Expr... I>
  struct Item:Chain<I...,Empty>::template To<Obj<Item<I...>>> {
    using Base=typename Chain<I...,Empty>::template To<Obj<Item<I...>>>;
    using This=Item<I...>;
    using Base::Base;
  };

  template<typename F,typename S=Empty<Nil>>
  struct Pair:F {
    using Base=F;
    using This=Pair<F,S>;
    using F::F;
    S tail;
    using F::changed;

    template<typename A,Idx i>
    typename A::Result walkId(A& api) {
      return F::id(i)?api.template call<F>(*this):tail.template walkId<A,i>(api);
    }

    template<typename A>
    typename A::Result walkPath(A& api,PathRef ref,Idx n) {
      if (n) return tail.template walkPath<A>(api,ref,n-1);
      if (ref) return F::obj().template walkPath<A>(api,ref.tail(),ref.head());
      return api.template call<F>(*this);
    }

    inline constexpr size_t size() const {return tail.size()+1;}
    using F::parentPrint;
    inline bool parentPrint(Idx n) {return n?tail.parentPrint(n-1):F::parentPrint();}

    // template<Cmd c,typename Nav>
    // inline bool cmd(Nav& nav,PathRef ref,Idx n) {
    //   trace(MDO<<"Pair::cmd "<<c<<" ref:"<<ref<<" n:"<<n<<endl);
    //   if(n) return tail.template cmd<c,Nav>(nav,ref,n-1);
    //   if (ref) return F::obj().template cmd<c,Nav>(nav,ref.tail());
    //   return F::template cmd<c,Nav>(nav);
    // }
    //
    // template<Cmd c,typename Nav,Idx i>
    // inline bool cmd(Nav& nav) {
    //   trace(MDO<<"Pair::cmd "<<c<<" id:"<<o<<endl);
    //   return F::id(i)?F::template cmd<c,Nav>(nav):tail.template cmd<c,Nav,i>(nav);
    // }
    //
    // inline void changed(Idx i,bool o) {
    //   if (i) tail.changed(i-1,o);
    //   else F::changed(o);
    // }
    //
    // template<Idx i>
    // inline void changed(bool o) {
    //   if (F::id(i)) F::changed(o);
    //   else tail.template changed<i>(o);
    // }
    //
    // inline bool enabled(PathRef ref=self,Idx n=0) const {
    //   if(n) return tail.enabled(ref,--n);
    //   if (ref) return F::enabled(ref.tail());
    //   return F::enabled();
    // }
    //
    // template<Idx i>
    // inline bool enabled() const {
    //   return F::id(i)?F::enabled():tail.template enabled<i>();
    // }
    //
    // inline void enable(bool b,PathRef ref=self,Idx n=0) {
    //   if(n) tail.enable(b,ref,--n);
    //   else if (ref) F::enable(b,ref.tail());
    //   else F::enable(b);
    // }
    //
    // template<Idx i>
    // inline void enable(bool b) {
    //   if(F::id(i)) F::enable(b);
    //   else tail.template enable<i>(b);
    // }
    //
    // inline Idx size(PathRef ref=self,Idx n=0) const {
    //   if(n) return tail.size(ref,--n);
    //   if (ref) return F::size(ref.tail());
    //   return tail.size()+1;
    // }
    //
    // template<Idx i>
    // inline constexpr Idx size() const {
    //   return F::id(i)?size():tail.template size<i>();
    // }
    //
    // inline bool canNav(PathRef ref=self,Idx n=0) {
    //   if(n) return tail.canNav(ref,--n);
    //   if (ref) return F::canNav(ref.tail());
    //   return F::canNav();
    // }
    //
    // template<Idx i>
    // inline constexpr bool canNav() {
    //   return F::id(i)?F::canNav():tail.template canNav<i>();
    // }
    //
    // inline ActRes activate(PathRef ref=self,Idx n=0) {
    //   if(n) return tail.activate(ref,--n);
    //   if (ref.len==1&&!F::enabled()) return F::activate();
    //   if (ref) return F::activate(ref.tail());
    //   return F::activate();
    // }
    //
    // template<Idx i>
    // inline ActRes activate() {
    //   return F::id(i)?F::activate():tail.template activate<i>();
    // }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out) {
      out.template printMenu<typename F::Type,Nav,op>
        (F::obj(),nav,op==Op::Printing&&(out.fullDraw()||F::changed()||out.isSame(&F::obj())));
    }

    // template<typename Nav,typename Out,Op op=Op::Printing,Idx i>
    // void printMenu(Nav& nav,Out& out);
    //
    // template<typename Nav,typename Out,Op op=Op::Printing>
    // void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0);

    template<typename Nav,typename Out,Op op=Op::Printing>
    void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0) {
      trace(MDO<<"Pair<...>::printItems out:@"<<(unsigned long)&out<<" out.fullDraw:"<<out.fullDraw()<<endl);
      if (op==Op::Printing&&(fullPrint||out.fullDraw()||F::changed()))
        out.template printItem<typename F::Type,Nav,op>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
      else out.template printItem<
          typename F::Type,
          Nav,op==Op::Printing?Op::Measure:op
        >(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
      tail.template printItems<Nav,Out,op>(nav,out,fullPrint,idx+1,top);
    }
  };

  template<typename Title,typename Body>
  struct StaticMenu {
    template<typename I>
    struct Part:I {
      using Base=I;
      Title title;
      Body body;
      using This=StaticMenu<Title,Body>;
      using I::I;

      template<typename A>
      typename A::Result walkPath(A& api,PathRef ref) {
        return api.chk(*this,ref)?
          body.template walkPath<A>(api,ref,ref.head()):
          api.call(*this);
      }

      // inline bool enabled(PathRef ref=self) const {return ref?body.enabled(ref,ref.head()):Base::enabled();}
      // inline void enable(bool b,PathRef ref=self) {if(ref) body.enable(b,ref,ref.head()); else Base::enable(b);}
      inline constexpr size_t size() const {return body.size();}
      // inline size_t size(PathRef ref=self) const {return ref?body.size(ref,ref.head()):body.size();}
      inline static constexpr bool canNav() {return true;}
      // inline bool canNav(PathRef ref=self) {return ref?body.canNav(ref,ref.head()):true;}
      inline ActRes activate() {return ActRes::Open;}
      // inline ActRes activate(PathRef ref=self) {return ref?body.activate(ref,ref.head()):Base::enabled()?ActRes::Open:ActRes::Stay;}
      using Base::parentPrint;
      inline bool parentPrint(Idx n) {return body.parentPrint(n);}
      // inline bool parentPrint(PathRef ref=self) {return ref?body.parentPrint(ref,ref.head()):false;}

      using Base::changed;
      // inline void changed(Idx i,bool o,PathRef ref) {body.changed(i,o,ref);}
      // inline void changed(Idx i,bool o) {
      //   trace(MDO<<"StaticMenu::changed("<<i<<","<<o<<")"<<endl);
      //   body.changed(i,o);}

      // template<Cmd c,typename Nav>
      // inline bool cmd(Nav& nav,PathRef ref=self) {
      //   trace(MDO<<"StaticMenu::cmd "<<c<<" ref:"<<ref<<endl);
      //   if(ref.len==1) {
      //     Idx p=nav.pos();
      //     bool res=body.template cmd<c,Nav>(nav,ref,ref.head());
      //     if(p!=nav.pos()) {
      //       changed(p,true);
      //       changed(nav.pos(),true);
      //     }
      //     return res;
      //   }
      //   return ref?body.template cmd<c,Nav>(nav,ref,ref.head()):Base::template cmd<c,Nav>(nav,ref);
      // }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out) {
        trace(MDO<<"StaticMenu::printMenu"<<endl);
        out.template printMenu<typename I::Type,Nav,op>
          (Base::obj(),nav,out.fullDraw()||!out.isSame(&Base::obj()));
      }

      // template<typename Nav,typename Out,Op op=Op::Printing>
      // inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
      //   trace(MDO<<"StaticMenu::printMenu... "<<op<<" "<<ref<<endl);
      //   if(ref.len>1&&body.parentPrint(ref))
      //     body.template printMenu<Nav,Out,op>(nav,out,ref.tail(),ref.head());
      //   else out.template printMenu
      //     <typename I::Type,Nav,op>
      //     (Base::obj(),nav,out.fullDraw()||!out.isSame(&Base::obj()));
      // }

      // template<typename Nav,typename Out,Op op=Op::Printing,Idx i>
      // inline void printMenu(Nav& nav,Out& out) {
      //   if (Base::obj().id(i))
      //     out.template printMenu<typename I::Type,Nav,op>
      //       (Base::obj(),nav,out.fullDraw()||!out.isSame(&Base::obj()));
      //   else body.template printMenu<Nav,Out,op,i>(nav,out);
      // }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0) {
        trace(MDO<<"StaticMenu::printItems"<<endl);
        body.template printItems<Nav,Out,op>(nav,out,fullPrint,idx,top);
      }

      // template<typename Nav,typename Out,Op op=Op::Printing>
      // inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0,PathRef ref=self) {
      //   trace(MDO<<"StaticMenu::printItems fullPrint:"<<fullPrint<<" out:"<<((long)&out)<<endl);
      //   body.template printItems<Nav,Out,op>(nav,out,fullPrint,idx,top,ref);
      // }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printTitle(Nav& nav,Out& out,bool fullPrint) {
        trace(MDO<<"StaticMenu::printTitle "<<op<<" fullDraw:"<<out.fullDraw()<<" changed:"<<title.changed()<<" out:"<<((long)&out)<<endl);
        if (op==Op::ClearChanges) title.changed(false);
        else {
          if (op==Op::Printing&&(fullPrint||out.fullDraw()||title.changed()))
            out.template printTitle<typename I::Type,Nav,op>(I::obj(),nav);
          else
            out.template printTitle<typename I::Type,Nav,op==Op::Printing?Op::Measure:op>(I::obj(),nav);
        }
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out) {
        trace(MDO<<"StaticMenu::print "<<op<<endl);
        title.template print<Nav,Out,op>(nav,out);
      }

      // template<typename Nav,typename Out,Op op=Op::Printing,Idx i>
      // inline void print(Nav& nav,Out& out) {
      //   title.template print<Nav,Out,op,i>(nav,out);
      // }
    };
  };

  template<typename O,typename... OO> struct StaticData:Pair<O,StaticData<OO...>> {};
  template<typename O>                struct StaticData<O>:Pair<O> {};
};