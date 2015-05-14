#ifndef FX_H
#define FX_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QTreeView>

//#include <jack/ringbuffer.h>


#include "3pass_eq.h"

enum FxState {
              FXS_STOP,
              FXS_RUN,
              FXS_PAUSE
              };


class CRingbuffer
{
public:

   float *buffer;
   size_t samples_max; //buffer size in samples
   size_t current_size; //how manu samples are in the buffer?
   size_t head; //index
   size_t tail; //index
   size_t ret_value;
   
   CRingbuffer (size_t samples, float def_value = 0);
   void prepare_buffer (size_t samples);
   ~CRingbuffer();
   
   void put (float f);
   float get();
};


class AFx: public QObject
{
  Q_OBJECT

public:
  
  FxState state;
  bool bypass;
  bool realtime;

  bool ui_visible;
  
  float *buffer;  //inner buffer for some purposes
  
  int samplerate;
  int channels;

  QWidget *wnd_ui;
  
  QString name;

  AFx (int srate, int chann);
  virtual ~AFx();

  virtual size_t execute (float *samples, size_t length) = 0;
  virtual void set_state (FxState s);
  virtual void reset_params (int srate, int chann);

//  virtual void load_from_string (const QString &s) = 0;
//  virtual QString save_to_string() = 0;

  void show_ui();

  //virtual void set_parameter (const QString &param, const QString &value) = 0;

  virtual AFx* self_create (int srate, int chann) = 0;
};


class CFxSimpleEQ: public AFx
{
 Q_OBJECT

public:

  EQSTATE eq_state_ch00;
  EQSTATE eq_state_ch01;

  CFxSimpleEQ (int srate, int chann);
  ~CFxSimpleEQ();

  AFx* self_create (int srate, int chann);

  size_t execute (float *samples, size_t length);

public slots:

  void dial_low_valueChanged (int value);
  void dial_mid_valueChanged (int value);
  void dial_hi_valueChanged (int value);

 // void load_from_string (const QString &s) {};
  //QString save_to_string()  {};

};


class CFxSimpleAmp: public AFx
{
  Q_OBJECT

public:

  float gain;

  CFxSimpleAmp (int srate, int chann);
  ~CFxSimpleAmp();

  QLabel *label;

  AFx* self_create (int srate, int chann);

  size_t execute (float *samples, size_t length);

  //void load_from_string (const QString &s);
  //QString save_to_string();


public slots:

  void dial_gain_valueChanged (int value);
};


class CFxSimpleOverdrive: public AFx
{
  Q_OBJECT

public:

  float gain;

  CFxSimpleOverdrive (int srate, int chann);
  ~CFxSimpleOverdrive();

  AFx* self_create (int srate, int chann);

  size_t execute (float *samples, size_t length);

public slots:

  void dial_gain_valueChanged (int value);
};


class CFxSimpleNoiseGate: public AFx
{
  Q_OBJECT

public:

  float level; //in samples

  CFxSimpleNoiseGate (int srate, int chann);

  AFx* self_create (int srate, int chann);

  size_t execute (float *samples, size_t length);

public slots:

  void	levelChanged (double d);
};


class CFxPitchShift: public AFx
{
  Q_OBJECT

public:

  QLabel *label;
  double ratio;

  CFxPitchShift (int srate, int chann);
  ~CFxPitchShift();

  AFx* self_create (int srate, int chann);

  size_t execute (float *samples, size_t length);

public slots:

  void spb_ratio_changed (double value);
};


class CFxSimpleLimiter: public AFx
{
  Q_OBJECT

public:

  float threshold; //in samples

  CFxSimpleLimiter (int srate, int chann);

  AFx* self_create (int srate, int chann);

  size_t execute (float *samples, size_t length);

public slots:

  void	levelChanged (double d);
};


class CFxSimpleDelay: public AFx
{
  Q_OBJECT

public:

  double dfactor;

  float delay; //in samples

  float current_delay_sample; //in samples

  //CRingbuffer *ringbuffer;

  //jack_ringbuffer_t *ringbuffer;

  CFxSimpleDelay (int srate, int chann);
  ~CFxSimpleDelay();

  AFx* self_create (int srate, int chann);
  void set_state (FxState s);

  void reset_params (int srate, int chann);

  size_t execute (float *samples, size_t length);

public slots:

  void delayChanged (double d);
};



//available fx
class CFxList: public QObject
{
public:

 QList < AFx *> list;

 CFxList();
 ~CFxList();
 
 AFx *find_by_name (const QString &fxname);
 QStringList names();
};



#endif // FX_H
