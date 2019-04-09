/* -*- C++ -*- */
#pragma once

// #include "../menu.h"
#include "base.h"
#include "out.h"

namespace AM5 {

  //can we config like this?
  // using Config=AM5::Config<
  //   wrapMenus::yes,
  //   canExit::yes,
  //   useFields::yes,
  //   useActions::yes,
  //   itemBase<AM5::Nil>,
  //   outBase<AM5::Nil>,
  //   manageIdle::no,
  //   navStyle<AM5::FlatNav>,
  //   titleWrapChars<'[',']'>,
  //   textCursors<' ','>','-'>
  // } myConfig;

  //not sure if i implement This
  //it will grow the vtable footprint a bit
  //or maybe not as it is only requested for containers
  //and we only need 1 virtual to get an iterator
  // template<typename I>
  // struct ItemIterator {
  //   size_t at=0;
  //   virtual inline I operator*() {return data[at];}
  //   virtual inline I operator==(const It o) const {return at==o.at;}
  //   virtual inline I operator!=(const It o) const {return !operator==(o);}
  //   virtual inline I operator++() {return data[++at];}
  //   virtual inline I operator++(I) {return data[at++];}
  // };

  //events should be flagged start/end
  //this should be used only for drawing custom items
  //still is preferable to extend a class
  //using this can encourage users to allocate/deallocate resources
  //and that does not play well with async request
  //as for web
  //for compatinilioty we should have them
  // make them optional thou!
  // enum class Events {
  //   enterEvent,
  //   exitEvent,
  //   updateEvent,
  //   focusEvent,
  //   blurEvent,
  //   ...
  // };

  enum class Styles {
    CanNav,//nav commands should be send to this item
    Wrap,//this menu wraps (numFields can look at this too)
    Action,//can receive enter
  };

  ///////////////////////////////////////////////////////////////
  // config options -----------------------------------

  // template
  // struct ItemNavCfg {
  //   virtual bool canNav() const {return false;}//TODO: use flags/properties field to reuse this virtual
  //   virtual bool up() {return false;}
  //   virtual bool down() {return false;}
  //   virtual bool enter() {return false;}
  //   virtual bool esc() {return false;}
  // };

  ///////////////////////////////////////////////////////////////
  // menu items -----------------------------------

  #if NAV_AGENT
    struct CmdAgent {
      // CmdAgent(Item& o):client(o) {}
      inline operator bool() const {return canNav();}
      // inline operator Item&() const {return getClient();}
      inline virtual bool canNav() const {return false;}
      inline virtual bool up() {return false;}
      inline virtual bool down() {return false;}
      inline virtual bool enter(){return false;}
      inline virtual bool esc() {return false;}
    };
    using NavRes=CmdAgent;
  #else
    #define CmdAgent() (false)
    using NavRes=bool;
  #endif

  // template<typename Cfg=ItemNavCfg>
  struct Item {
    //footprint:
    // 4 bytes for each virtual function * #virtual tables
    // the # of vtables is equal to the # of unique Prompt<...> compositions
    virtual void out(MenuOut& o) const {}
    #if (MENU_INJECT_PARTS==true)
      virtual void out(MenuOut& o,PrinterPart& pp) const {}
    #endif
    virtual size_t size() const {return 1;}
    virtual Item& operator[](size_t)=0;// const {return *this;}
    virtual NavRes navAgent()=0;// {assert(false);return CmdAgent();};
    #if !NAV_AGENT
      inline virtual bool canNav() const {return false;}
      inline virtual bool up() {return false;}
      inline virtual bool down() {return false;}
      inline virtual bool enter(){return false;}
      inline virtual bool esc() {return false;}
    #endif
  };

  #if NAV_AGENT
    template<typename O>
    class ItemAgent:public CmdAgent {
      public:
        ItemAgent(O& o):client(o) {}
        inline bool canNav() const override {return true;}
        inline bool up() override {return client.up();}
        inline bool down() override {return client.down();}
        inline bool enter() override{return client.enter();}
        inline bool esc() override {return client.esc();}
      protected:
        O& client;
    };
  #else
    #define ItemAgent(...) (true)
  #endif

  //adapt specific types as menu items
  //provide virtual overrides for them
  template<typename O>
  struct Prompt:public virtual Item,public O {
    using O::O;
    using This=Prompt<O>;
    inline void out(MenuOut& o) const override {O::out(o);}
    #if (MENU_INJECT_PARTS==true)
      void out(MenuOut& o,PrinterPart& pp) const override;
    #endif
    size_t size() const override {return O::size();}
    Item& operator[](size_t n) override {return O::operator[](n);}
    inline NavRes navAgent() override {return O::navAgent();}
    #if !NAV_AGENT
      inline bool canNav() const override {return true;}
      inline bool up() override {return O::up();}
      inline bool down() override {return O::down();}
      inline bool enter() override{return O::enter();}
      inline bool esc() override {return O::esc();}
    #endif
    template<template<typename> class T>
    inline void stack(MenuOut& o) const {Prompt<T<O>>(*this).out(o);}
  };

  #if (MENU_INJECT_PARTS==true)
    struct PrinterPart {
      template<typename O>
      void use(MenuOut& o,Prompt<O>& i) {
        // Serial<<"PrinterPart::use..."<<endl;
        Prompt<O>(i).out(o);
        // i.stack<part>(o);
      }
    };
  #endif

  #if (MENU_INJECT_PARTS==true)
    template<typename O>
    void Prompt<O>::out(MenuOut& o,PrinterPart& pp) const {
      pp.use<O>(o,*this);
    }
  #endif

  //static composition blocks -----------------------
  struct Empty {
    inline Empty() {}
    inline Empty(Empty&) {}
    static inline void out(MenuOut&) {}
    static inline size_t size() {return 1;}
    inline Item& operator[](size_t n) {return *reinterpret_cast<Item*>(this);}
    static inline NavRes navAgent() {return CmdAgent(); }
    static inline bool up() {return false;}
    static inline bool down() {return false;}
    static inline bool enter() {return false;}
    static inline bool esc() {return false;}
  };

  template<typename O>
  struct Text:public O {
    const char* text;
    inline Text(const char* text):text(text) {}
    inline Text(const Text<O>& t):text(t.text) {}
    inline void out(MenuOut &o) const {
      o<<text;
      O::out(o);
    }
  };

  template<size_t n,typename O>
  class StaticMenu:public O {
  protected:
    Item* data[n];
  public:
    template<typename... OO>
    inline StaticMenu(OO... oo):data{oo...} {}
    template<typename... OO>
    inline StaticMenu(const char*title,OO... oo):O(title),data{oo...} {}
    static inline size_t size() {return n;}
    inline Item& operator[](size_t i) {return *data[i];}
  };

};//AM5
