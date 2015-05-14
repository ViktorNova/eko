/*
this code is Public Domain
Peter Semiletov
*/

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QHash>
#include <QFileInfo>
#include <QStringList>

#include <math.h>

class CFilesList: public QObject
{
public:

  QStringList list;

  void get (const QString &path);
  void iterate (QFileInfo &fi);
};


class CFTypeChecker: public QObject
{
public:

  QStringList lexts;
  QStringList lnames;

  CFTypeChecker (const QString &fnames, const QString &exts);
  bool check (const QString &fname);
};


QString hash_keyval_to_string (const QHash<QString, QString> &h);
QString hash_get_val (QHash<QString, QString> &h, const QString &key, const QString &def_val);

QString file_get_ext (const QString &file_name);

bool file_exists (const QString &fileName);
void qstring_list_print (const QStringList &l);

bool qstring_save (const QString &fileName, const QString &data, const char *enc = "UTF-8");
QString qstring_load (const QString &fileName, const char *enc = "UTF-8");

QStringList read_dir_entries (const QString &path);
QHash<QString, QString> hash_load (const QString &fname);
QHash<QString, QString> hash_load_keyval (const QString &fname);
QHash<QString, QString> stringlist_to_hash (const QStringList &l);
QString hash_keyval_to_string (QHash<QString, QString> *h);
bool is_image (const QString &filename);
QString string_between (const QString &source, const QString &sep1, const QString &sep2);
QByteArray file_load (const QString &fileName);


QString change_file_ext (const QString &s, const QString &ext);
//QString get_insert_image (const QString &file_name, const QString &full_path, const QString &markup_mode);



inline int get_value (int total, int perc)
{
  return static_cast <int> (total * perc / 100);
}


inline float get_fvalue (float total, float perc)
{
  return (total * perc / 100);
}


inline double get_percent (double total, double value)
{
  return (value / total) * 100;
}


inline float get_percent (float total, float value)
{
  return (value / total) * 100;
}


inline bool is_dir (const QString &path)
{
  return QFileInfo(path).isDir();
}


inline QString get_file_path (const QString &fileName)
{
  return QFileInfo (fileName).absolutePath();
}

inline float float2db (float v)
{
  if (v == 0)
     return 0;
         
  if (v > 0)
     return (float) 20 * log10 (v / 1.0);

  return (float) 20 * log10 (v / -1.0);
}


/* from 
 *  db.h
 *
 *  Copyright (C) 2003,2005 Steve Harris, Nicholas Humfrey
 *
 */
static inline float
db2lin( float db )
{
        if (db <= -90.0f) return 0.0f;
        else {
                return powf(10.0f, db * 0.05f);
        }
}


static inline float
lin2db( float lin )
{
        if (lin == 0.0f) return -90.0f;
        else return (20.0f * log10f(lin));
}


double input_double_value (const QString &caption, const QString &lbl,
                           double minval, double maxval, double defval, double step);



size_t round_to (size_t value, size_t to, bool inc);


#endif
