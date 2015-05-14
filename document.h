/***************************************************************************
 *   2010-2015 by Peter Semiletov                            *
 *   tea@list.ru                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   aint with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef DOCUMENT_H
#define DOCUMENT_H


#include <QObject>

#include <QHash>
#include <QLabel>
#include <QStatusBar>
#include <QMainWindow>
#include <QScrollBar>
#include <QTimer>

#include <portaudio.h>


#include "logmemo.h"
#include "tio.h"
#include "fx.h"


/*
mapping for mp3/wav/flac:

SPEAKER_FRONT_LEFT = 0,
SPEAKER_FRONT_RIGHT,
SPEAKER_FRONT_CENTER,
SPEAKER_LOW_FREQUENCY, //subwoofer
SPEAKER_BACK_LEFT,
SPEAKER_BACK_RIGHT,
SPEAKER_FRONT_LEFT_OF_CENTER,
SPEAKER_FRONT_RIGHT_OF_CENTER,
SPEAKER_BACK_CENTER,
SPEAKER_SIDE_LEFT,
SPEAKER_SIDE_RIGHT,
SPEAKER_LEFT_HEIGHT,
SPEAKER_RIGHT_HEIGHT
*/

enum EChannels
{
CH_FRONT_LEFT = 0,
CH_FRONT_RIGHT,
CH_FRONT_CENTER,
CH_LOW_FREQUENCY, //subwoofer
CH_BACK_LEFT,
CH_BACK_RIGHT,
CH_FRONT_LEFT_OF_CENTER,
CH_FRONT_RIGHT_OF_CENTER,
CH_BACK_CENTER,
CH_SIDE_LEFT,
CH_SIDE_RIGHT,
CH_LEFT_HEIGHT,
CH_RIGHT_HEIGHT
};

/*
5.1 WAV front left channel front right channel front center channel LFE rear left channel rear right channel
5.1 AC3 front left channel front center channel front right channel rear left channel rear right channel LFE
5.1 DTS front center channel front left channel front right channel rear left channel rear right channel LFE
5.1 AAC front center channel front left channel front right channel rear left channel rear right channel LFE
5.1 AIFF front left channel rear left channel front center channel front right channel rear right channel LFE
*/


enum ETransportStates
{
  STATE_STOP = 0,
  STATE_PLAY,
  STATE_PAUSE,
  STATE_EXIT,
  STATE_RECORD
};


enum EUndoModes
{
  UNDO_UNDEFINED = 0,
  UNDO_DELETE,
  UNDO_PASTE,
  UNDO_MODIFY,
  UNDO_WHOLE,
  UNDO_INSERT
};


class CDocumentHolder;
class CDocument;



class CTransportControl: public QObject
{
Q_OBJECT

signals:

  void play_pause();
  void stop();

public:

  void call_play_pause();
  void call_stop();
};


class CSoundBuffer: public QObject
{
  Q_OBJECT

public:

  size_t buffer_offset;
  size_t buffer_size;
  
  int channels;
  int samplerate;

  size_t samples_total;
  size_t frames;

  bool use_sndfile_format;
  int sndfile_format;

  float *buffer;
  
  CSoundBuffer (QObject *parent = 0);
  ~CSoundBuffer();

  void copy_from (CSoundBuffer *other);
  void copy_from_w_resample (CSoundBuffer *other);

  CSoundBuffer* copy_to (size_t sample_start, size_t sample_end);
  void delete_range (size_t sample_start, size_t sample_end);
  void paste_at (CSoundBuffer *other, size_t pos);
  void overwrite_at (CSoundBuffer *other, size_t pos);
  
  CSoundBuffer* resample (int new_rate);
  CSoundBuffer* convert_to_mono();
  CSoundBuffer* convert_to_stereo (bool full);
  CSoundBuffer* convert_ch6_to_stereo (int algo = 0);

  void copy_params (CSoundBuffer *sb);
};


class CUndoElement: public QObject
{
  Q_OBJECT

public:
  
  int sndfile_format;
  bool use_sndfile_format;
  int channels;
  int samplerate; 
  
  int type;
  
  bool selected;
    
  size_t frames_per_section;

  size_t sample_start; //samples, selection start
  size_t sample_end; //samples, selection end
  size_t sample_cursor;

  CSoundBuffer *buffer;
  
  CUndoElement();
  ~CUndoElement();
};


class CChannelSamples
{
public:

  float *samples;
  float temp;
  
  CChannelSamples (size_t size);
  ~CChannelSamples(); 
};


class CChannelMinmax
{
public:

  CChannelSamples *min_values;
  CChannelSamples *max_values;
      
  CChannelMinmax (size_t size);
  ~CChannelMinmax(); 
};


class CMinmaxes
{
public:

  CChannelMinmax **values;
    
  size_t count;
  
  CMinmaxes (size_t size, size_t sections);
  ~CMinmaxes(); 
};


class CWaveform;


class CTimeRuler: public QWidget
{
  Q_OBJECT

public:

  CWaveform *waveform;
  bool init_state;

  QColor background_color;
  QColor foreground_color;

  CTimeRuler (QWidget *parent);

protected:

  void paintEvent (QPaintEvent *event);
};


class CWaveEdit;


class CWaveform: public QWidget
{
  Q_OBJECT

public:

  //top-level link:
  CWaveEdit *wave_edit;
  

  CSoundBuffer *sound_buffer;
  CTimeRuler *timeruler;


  bool show_db; //false if floats
  bool play_looped;
   
  CMinmaxes *minmaxes;   
  QScrollBar *scrollbar;

  QTimer timer;

  bool init_state;
  bool draw_shadow;

  bool mouse_pressed;
  bool normal_cursor_shape;
  bool selected;

  QImage waveform_image;

  float scale_factor;

  int cursor_position; //in sections

  int selection_start; //in sections
  int selection_end;  //in sections
  
  size_t sel_start_samples;
  size_t sel_end_samples;
  size_t cursor_samples;

  size_t frames_per_section;

  size_t section_from;
  size_t section_to;
  size_t sections_total;

//colors

  QColor cl_waveform_background;
  QColor cl_waveform_foreground; 
  QColor cl_waveform_selection_foreground; 
  QColor cl_axis; 
  QColor cl_cursor; 
  QColor cl_text; 
  QColor cl_shadow; 

  int waveform_selection_alpha;

  void load_color (const QString &fname);

//end of colors

  
  CWaveform (QWidget *parent);
  ~CWaveform();

  void set_cursor_value (size_t section);
  void set_selstart_value (size_t section);
  void set_selend_value (size_t section);

  void set_statusbar_text();
  void set_cursorpos_text();

  void scale (int delta);

  void recalc_view();
  void prepare_image();

  void magic_update();
  void fix_selection_bounds();
  
  void deselect();
  void select_all();
  
  void copy_selected();
  void delete_selected();
  void cut_selected();
    
  void paste();
  
  size_t sample_start(); //in samples, not frames
  size_t sample_end();   //in samples

  size_t cursor_pos_sample();
  void set_cursor_to_sample (size_t sampleno);

//undo/redo stuff
  QList <CUndoElement*> undos;
  int max_undos;
//  int current_undo;
 
  void undo_take_shot (int type, int param = 0); 
  void undo_top(); //simple test of the undo

  void flush_undos();

  void redo();
  
  void update_cursor_pos();  
  
  
protected:

//  void dragEnterEvent (QDragEnterEvent *event);
//  void dropEvent (QDropEvent *event);
  void paintEvent(QPaintEvent *event);
  void mousePressEvent (QMouseEvent *event);  
  void wheelEvent (QWheelEvent *event );  
  void keyPressEvent (QKeyEvent *event);    
  void resizeEvent(QResizeEvent *event);
  
  void mouseMoveEvent (QMouseEvent *event); 
  void mouseReleaseEvent (QMouseEvent * event);  
  
  void mouseDoubleClickEvent ( QMouseEvent * event);
  
public slots:

  void timer_timeout();  
  
};


class CWaveEdit: public QWidget
{
  Q_OBJECT

public:

  CDocument *doc;
  CWaveform *waveform;
  
  QScrollBar *scb_horizontal;
 
  CTimeRuler *timeruler;

  CWaveEdit (QWidget *parent = 0);
  ~CWaveEdit();

  bool isReadOnly();
  
protected:

  void dragEnterEvent (QDragEnterEvent *event);
  void dropEvent (QDropEvent *event);
  
public slots:

  void scb_horizontal_valueChanged (int value);  
  
};


class CDocument: public QObject
{
  Q_OBJECT

public:

  QHash <QString, QString> fnameswoexts;
    
  CDocumentHolder *holder;
  

  CWaveEdit *wave_edit;

  QString file_name;
  QWidget *tab_page;
  
  size_t position;

  CDocument (QObject *parent = 0);
  ~CDocument();

  void paste();
  
  void create_new();
  
  void set_tab_caption (const QString &fileName);
  
  bool save_with_name (const QString &fileName);
  bool save_with_name_plain (const QString &fileName);
  void goto_pos (size_t pos);
  
  bool open_file (const QString &fileName, bool set_fname = true);
  void reload();
  int get_tab_idx();
  QString get_triplex();
  void update_title (bool fullname = true);

  void effects_state_save();
  void effects_state_restore();
  void effects_close_all();
  void effects_update_params();
};


class CDocumentHolder: public QObject
{
  Q_OBJECT

public:

  CTioHandler tio_handler;

  QString fname_current_session;

  QHash <QString, QString> palette;

  QLabel *l_status_bar;
  QLabel *l_maintime; 
  
  CTransportControl *transport_control;  


  CDocument *current;
  
  QString dir_config;
  
  QString def_palette;

  QStatusBar *status_bar;
  CLogMemo *log;
  QList <CDocument*> list;
  QMainWindow *parent_wnd;
  QTabWidget *tab_widget;

  QMenu *recent_menu;
  QStringList recent_files;
  QString recent_list_fname;

  CDocumentHolder (QObject *parent = 0);
  ~CDocumentHolder();

  CDocument* create_new();
  CDocument* open_file (const QString &fileName, bool set_fname = true);
  void reload_recent_list();

  void save_to_session (const QString &fileName);
  void load_from_session (const QString &fileName);
  void load_palette (const QString &fileName);

  //void close_by_idx (int i);
  void close_current();
  void apply_settings();
  void apply_settings_single (CDocument *d);

  void add_to_recent (CDocument *d);
  void update_recent_menu();

  CDocument* get_current();

  public slots:

  void open_recent();
};




class CVLevelMeter;
class CFxRack;

class CDSP: public QObject
{

Q_OBJECT

public:
 
  float maxl; //for the current nframes
  float maxr;

  float temp_buffer [8192];

  CDSP (QObject *parent = 0);
  ~CDSP() {};
  
  
  bool process_whole_document (CDocument *d);
  size_t process (size_t nframes);
  size_t process_rec (float *buffer, size_t channels, size_t nframes);

};


class CVLevelMeter: public QWidget
{
  Q_OBJECT

public:
  
  bool init_state;

  QImage img_bar;
 
  int scale_width;
  int bars_width;
  
  float peak_l;
  float peak_r;
  float pl;
  float pr;

  CVLevelMeter (QWidget *parent);

  void update_scale_image();

protected:

 void paintEvent (QPaintEvent *event);
 void resizeEvent(QResizeEvent *event);
};


class CFxRackWindow: public QWidget
{
Q_OBJECT

public:

  CVLevelMeter *level_meter;
  QTimer tm_level_meter;
  CFxRack *fx_rack;

  CFxRackWindow();
  ~CFxRackWindow();

public slots:

  void tm_level_meter_timeout();
  void apply_fx();

protected:

  void closeEvent (QCloseEvent *event);
};



class CFxTreeView;
class CFxListInterface;


class CFxRack: public QObject
{
  Q_OBJECT

public:

  CFxList *avail_fx;

  QList <AFx*> effects;
  

  QWidget *inserts; //inserts section for the mixer, the "external" widget

  QStandardItemModel *model;

  CFxTreeView *tree_view; //inside of the inserts widget

  CFxRack (QObject *parent = 0);
  ~CFxRack();

  void add_entry (AFx *f, bool checked = true);
  void ins_entry (AFx *f);

  const QModelIndex index_from_name (const QString &name);
  const QModelIndex index_from_idx (int idx);
  int get_sel_index();

  void bypass_all (bool mode = true);
  void set_state_all (FxState state);

  QString get_sel_fname();
  void print_fx_list();

public slots:

  void tv_activated (const QModelIndex &index);
  void add_fx();
  void del_fx();

  void bt_up_clicked();
  void bt_down_clicked();

  void bypass_dataChanged (const QModelIndex &topLeft, const QModelIndex &bottomRight);

signals:

 void fx_activated (const QString &path);
};


class CFxTreeView: public QTreeView
{
  Q_OBJECT

public:

 // CFxRack *rack;

protected:

  void mouseMoveEvent (QMouseEvent *event);
};



#endif
