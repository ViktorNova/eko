#include <QDebug>
#include <QProcess>

#include <sndfile.h>
#include <limits>

#include "tio.h"
#include "utils.h"


using namespace std;

double ogg_q;
CFileFormats *file_formats;


float* load_whole_sample (const char *fname, SF_INFO &sf)
{
  SNDFILE *file = sf_open (fname, SFM_READ, &sf);
  float *buffer = new float [sf.channels * sf.frames];
  sf_count_t zzz = sf_readf_float (file, buffer, sf.frames);
  sf_close (file);

  return buffer;
}


CTioPlainAudio::~CTioPlainAudio()
{
 // if (data)
  //   free (data);
}


float* CTioPlainAudio::load (const QString &fname)
{
  SF_INFO sf;
  sf.format = 0;
  
  SNDFILE *file = sf_open (fname.toUtf8().data(), SFM_READ, &sf);
  
  total_samples = sf.channels * sf.frames; 
  
  float *output_data  = new float [total_samples];
  sf_count_t zzz = sf_readf_float (file, output_data, sf.frames);
    
  sf_close (file);
  
  if (! output_data)
    {
     qDebug() << "! output_data";
     return NULL; 
    }  
  
  frames = sf.frames;
  samplerate = sf.samplerate;
  channels = sf.channels;
  format = sf.format;
/*
  qDebug() << "frames = " << frames;
  qDebug() << "total_samples = " << total_samples;
  qDebug() << "bytes = " << total_samples * sizeof (float);
  qDebug() << "MAX_INT = " << numeric_limits<int>::max();
*/
  return output_data;
}


bool CTioPlainAudio::save (const QString &fname)
{
  if (! input_data)
     {
      qDebug() << "! data";
      return false;
     } 

  SF_INFO sf;

  sf.samplerate = samplerate;
  sf.channels = channels;
  sf.format = format;
  
  if (! sf_format_check (&sf))
     {
      qDebug() << "! sf_format_check (&sf)";
      return false;  
     }  
  
  //if pcm 16 bit
  //bool CTioPlainAudio::save_16bit_pcm (const QString &fname)
  if ((sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_16) 
      return save_16bit_pcm (fname);

  SNDFILE *file = sf_open (fname.toUtf8().data(), SFM_WRITE, &sf);

  double quality = ogg_q;
  sf_command (file, SFC_SET_VBR_ENCODING_QUALITY, &quality, sizeof (quality)) ;

  //qDebug() << "ogg_q = " << ogg_q;

  sf_count_t zzz = sf_writef_float (file, input_data, frames);
  sf_close (file);

  return true;
}


CTioHandler::CTioHandler()
{
  list.append (new CTioPlainAudio);
  //list.append (new CTioMad);
  
  CTio *instance;

  for (int i = 0; i < list.size(); i++)
      {
       instance = list.at (i);
       supported_extensions += instance->extensions;
      }
}


CTioHandler::~CTioHandler()
{
  while (! list.isEmpty())
       delete list.takeFirst();
}


bool CTioHandler::is_ext_supported (const QString &fname)
{
  QString ext = file_get_ext (fname).toLower();

  if (supported_extensions.indexOf (ext) != -1)
     return true;

  return false;
}


CTio* CTioHandler::get_for_fname (const QString &fname)
{
  CTio *instance;
  QString ext = file_get_ext (fname).toLower();

  for (int i = 0; i < list.size(); i++)
      {
       instance = list.at (i);
       if (instance->extensions.indexOf (ext) != -1)
          return instance;
      }

  return NULL;
}


CTioPlainAudio::CTioPlainAudio()
{
  ronly = false;
  total_samples = 0;
  input_data = 0;
  
  frames = 0;
  samplerate = 0;
  channels = 0;
  format = 0;
  
  extensions += file_formats->hextensions.values();
}


CTioPlainAudio::CTioPlainAudio (bool rnly)
{
  ronly = rnly;
  total_samples = 0;
  input_data = 0;

  frames = 0;
  samplerate = 0;
  channels = 0;
  format = 0;
}


bool CTioReadOnly::save (const QString &fname)
{
  error_string = tr ("saving of this format is not supported");
  return false;
}


CFileFormats::CFileFormats()
{
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_IMA_ADPCM);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_MS_ADPCM);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_GSM610);

  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_IMA_ADPCM);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_MS_ADPCM);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_GSM610);
  
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_GSM610);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_DWVW_12);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_DWVW_16);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_DWVW_24);
  
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_G721_32);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_G723_24);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_G723_40);
  
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_GSM610);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_DWVW_12);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_DWVW_16);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_DWVW_24);
  
  hformat.insert (SF_FORMAT_PAF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_PAF, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_PAF, SF_FORMAT_PCM_24);
    
  hformat.insert (SF_FORMAT_SVX, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_SVX, SF_FORMAT_PCM_16);
  
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_ALAW);
  
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_ALAW);

  hformat.insert (SF_FORMAT_VOC, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_VOC, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_VOC, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_VOC, SF_FORMAT_ALAW);
  
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_IMA_ADPCM);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_MS_ADPCM);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_GSM610);
  
  hformat.insert (SF_FORMAT_MAT4, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_MAT4, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_MAT4, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_MAT4, SF_FORMAT_DOUBLE);
  
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_DOUBLE);
  
  hformat.insert (SF_FORMAT_PVF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_PVF, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_PVF, SF_FORMAT_PCM_32);
  
  hformat.insert (SF_FORMAT_XI, SF_FORMAT_DPCM_8);
  hformat.insert (SF_FORMAT_XI, SF_FORMAT_DPCM_16);
  
  hformat.insert (SF_FORMAT_HTK, SF_FORMAT_PCM_16);
  
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_ALAW);
  
  hformat.insert (SF_FORMAT_SD2, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_SD2, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_SD2, SF_FORMAT_PCM_24);
  
  hformat.insert (SF_FORMAT_FLAC, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_FLAC, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_FLAC, SF_FORMAT_PCM_24);


  hformat.insert (SF_FORMAT_OGG, SF_FORMAT_VORBIS);


  hformatnames.insert (SF_FORMAT_WAV, "Microsoft WAV format (little endian)");
  hformatnames.insert (SF_FORMAT_AIFF, "Apple/SGI AIFF format (big endian)");
  hformatnames.insert (SF_FORMAT_AU, "Sun/NeXT AU format (big endian)");
  hformatnames.insert (SF_FORMAT_RAW, "RAW PCM data");
  hformatnames.insert (SF_FORMAT_PAF, "Ensoniq PARIS file format");
  hformatnames.insert (SF_FORMAT_SVX, "Amiga IFF / SVX8 / SV16 format");
  hformatnames.insert (SF_FORMAT_NIST, "Sphere NIST format");
  hformatnames.insert (SF_FORMAT_VOC, "VOC files");
  hformatnames.insert (SF_FORMAT_IRCAM, "Berkeley/IRCAM/CARL");
  hformatnames.insert (SF_FORMAT_W64, "Sonic Foundry's 64 bit RIFF/WAV");
  hformatnames.insert (SF_FORMAT_MAT4, "Matlab (tm) V4.2 / GNU Octave 2.0");
  hformatnames.insert (SF_FORMAT_MAT5, "Matlab (tm) V5.0 / GNU Octave 2.1");
  hformatnames.insert (SF_FORMAT_PVF, "Portable Voice Format");
  hformatnames.insert (SF_FORMAT_XI, "Fasttracker 2 Extended Instrument");
  hformatnames.insert (SF_FORMAT_HTK, "HMM Tool Kit format");
  hformatnames.insert (SF_FORMAT_SDS, "Midi Sample Dump Standard");
  hformatnames.insert (SF_FORMAT_AVR, "Audio Visual Research");
  hformatnames.insert (SF_FORMAT_WAVEX, "MS WAVE with WAVEFORMATEX");
  hformatnames.insert (SF_FORMAT_SD2, "Sound Designer 2");
  hformatnames.insert (SF_FORMAT_FLAC, "FLAC lossless file format");
  hformatnames.insert (SF_FORMAT_CAF, "Core Audio File format");
  hformatnames.insert (SF_FORMAT_OGG, "Xiph OGG container");
    

  hextensions.insert (SF_FORMAT_WAV, "wav");
  hextensions.insertMulti (SF_FORMAT_AIFF, "aiff");
  hextensions.insertMulti (SF_FORMAT_AIFF, "aif");
  hextensions.insert (SF_FORMAT_AU, "au");
  hextensions.insert (SF_FORMAT_RAW, "raw");
  hextensions.insert (SF_FORMAT_PAF, "paf");
  hextensions.insert (SF_FORMAT_SVX, "iff");
  hextensions.insert (SF_FORMAT_NIST, "Sphere NIST format");//ext?
  hextensions.insert (SF_FORMAT_VOC, "voc");
  hextensions.insert (SF_FORMAT_IRCAM, "sf"); //not sound font
  hextensions.insert (SF_FORMAT_W64, "wav");
  hextensions.insert (SF_FORMAT_MAT4, "mat");
  hextensions.insert (SF_FORMAT_MAT5, "mat");
  hextensions.insert (SF_FORMAT_PVF, "pvf");
  hextensions.insert (SF_FORMAT_XI, "xi");
  hextensions.insert (SF_FORMAT_HTK, "htk");
  hextensions.insert (SF_FORMAT_SDS, "sds");
  hextensions.insert (SF_FORMAT_AVR, "avr");
  hextensions.insert (SF_FORMAT_WAVEX, "wav"); //see http://msdn.microsoft.com/en-us/library/dd757720%28VS.85%29.aspx
  hextensions.insert (SF_FORMAT_SD2, "sd2");
  hextensions.insert (SF_FORMAT_FLAC, "flac");
  hextensions.insert (SF_FORMAT_CAF, "caf");
  hextensions.insert (SF_FORMAT_OGG, "ogg");
      
  
  hsubtype.insert (SF_FORMAT_PCM_S8, "Signed 8 bit data");
  hsubtype.insert (SF_FORMAT_PCM_16, "Signed 16 bit data");
  hsubtype.insert (SF_FORMAT_PCM_24, "Signed 24 bit data");
  hsubtype.insert (SF_FORMAT_PCM_32, "Signed 32 bit data");
  hsubtype.insert (SF_FORMAT_PCM_U8, "Unsigned 8 bit data (WAV and RAW only)");
  hsubtype.insert (SF_FORMAT_FLOAT, "32 bit float data");
  hsubtype.insert (SF_FORMAT_DOUBLE, "64 bit float data");
  hsubtype.insert (SF_FORMAT_ULAW, "U-Law encoded");
  hsubtype.insert (SF_FORMAT_ALAW, "A-Law encoded");
  hsubtype.insert (SF_FORMAT_IMA_ADPCM, "IMA ADPCM");
  hsubtype.insert (SF_FORMAT_MS_ADPCM, "Microsoft ADPCM");
  hsubtype.insert (SF_FORMAT_GSM610, "GSM 6.10 encoding");
  hsubtype.insert (SF_FORMAT_VOX_ADPCM, "Oki Dialogic ADPCM encoding");
  hsubtype.insert (SF_FORMAT_G721_32, "32kbs G721 ADPCM encoding");
  hsubtype.insert (SF_FORMAT_G723_24, "24kbs G723 ADPCM encoding");
  hsubtype.insert (SF_FORMAT_G723_40, "40kbs G723 ADPCM encoding");
  hsubtype.insert (SF_FORMAT_DWVW_12, "12 bit Delta Width Variable Word encoding");
  hsubtype.insert (SF_FORMAT_DWVW_16, "16 bit Delta Width Variable Word encoding");
  hsubtype.insert (SF_FORMAT_DWVW_24, "24 bit Delta Width Variable Word encoding");
  hsubtype.insert (SF_FORMAT_DWVW_N, "N bit Delta Width Variable Word encoding");
  hsubtype.insert (SF_FORMAT_DPCM_8, "8 bit differential PCM (XI only)");
  hsubtype.insert (SF_FORMAT_DPCM_16, "16 bit differential PCM (XI only)");
  hsubtype.insert (SF_FORMAT_VORBIS, "Xiph Vorbis encoding");
}


void file_formats_init()
{
  file_formats = new CFileFormats; 
}


void file_formats_done()
{
  delete file_formats;
}


bool CTioPlainAudio::save_16bit_pcm (const QString &fname)
{
  if (! input_data)
     { 
      qDebug() << "! data";
      return false;
     } 

  SF_INFO	sf;

  sf.samplerate = samplerate;
  sf.channels = channels;
  sf.format = format;
  
  if (! sf_format_check (&sf))
     {
      qDebug() << "! sf_format_check (&sf)";
      return false;  
     }  

 qDebug() << "CTioPlainAudio::save_16bit_pcm";

//correctly convert data to 16-bit array

  size_t buflen = frames * sf.channels;
  
  short int *buf = new short int [buflen];
  
  for (size_t i = 0; i < buflen; i++)
      {
       float f = input_data[i];

       if (f >= 1.0)
          buf[i] = f * 32767;
       else
          buf[i] = f * 32768;
       }
   
  SNDFILE *file = sf_open (fname.toUtf8(), SFM_WRITE, &sf);
  
  sf_count_t zzz = sf_writef_short (file, buf, frames);
  
  sf_close (file);

  delete buf;

  return true;
}


float* load_from_lame (QString &fname)
{
 //lame '--decode'
  QString command;
  command = "lame --decode " + fname + " " + fname + "wav";
  int exit_code = QProcess::execute (command);
  qDebug() << exit_code;
  
  if (exit_code < 0)
     return 0;
}



/*

система кэширования:

класс

загрузить файл, конвертировать его в 32 бит float с частотой оцифровки подлинника
сохранить во временный файл filename.ext.tmp

предоставить функции чтения из этого временного файла в буфер и из буфера
по произвольному смещению

при вызове "сохранить исходный файл" конвертируем filename.ext.tmp в исходный файл

для построения волновой формы сканируем filename.ext.tmp


 */


/*
#ifdef MAD_ENABLED

CTioMad::CTioMad()
{
  extensions += "mp3";
}


CTioMad::~CTioMad()
{


}


float* CTioMad::load (const QString &fname)
{
  qDebug() << "CTioMad::load";

 // QFileInfo fi (fname);

  QString temp_fname = QDir::tempPath() + "/eko-temp-from-mp3.wav";

  QString program = "madplay " + QString ("\"") + fname + QString ("\"") + " -o wave:" + temp_fname;

  qDebug() << "temp_fname = " << temp_fname;
  //QStringList arguments;
//     arguments << "-style" << "motif";

  //QProcess *myProcess = new QProcess (this);
  //myProcess->start (program);

  if (file_exists (temp_fname))
  if (QProcess::execute (program))
     {
      CTioPlainAudio *pa = new CTioPlainAudio (true);
      float *f = pa->load (temp_fname);

      total_samples = pa->total_samples;
      frames = pa->frames;
      samplerate = pa->samplerate;
      channels = pa->channels;
      format = pa->format;

     // delete pa;

      return f;
     }
}


bool CTioMad::save (const QString &fname)
{


}


bool CTioMad::save_16bit_pcm (const QString &fname)
{


}
#endif
*/
