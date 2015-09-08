/*
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "config.h"
#include "help_mp.h"
#include "access_mpcontext.h"
#include "mixer.h"
#include "mpcommon.h"
#include "mp_core.h"
#include "path.h"
#include "sub/sub.h"
#include "sub/vobsub.h"
#ifdef CONFIG_ASS
#include "libass/ass_types.h"
#endif

#include "menu.h"
#include "dialog.h"
#include "gui/ui/actions.h"
#include "gui/app/app.h"
#include "gui/app/gui.h"
#include "gui/interface.h"

#include "stream/stream.h"
#include "libavutil/common.h"
#include "libmpcodecs/vd.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"
#include "libavutil/avstring.h"

#include "pixmaps/about.xpm"
#include "pixmaps/audio.xpm"
#include "pixmaps/video.xpm"
#include "pixmaps/half.xpm"
#include "pixmaps/normal.xpm"
#include "pixmaps/double.xpm"
#include "pixmaps/full.xpm"
#include "pixmaps/exit.xpm"
#include "pixmaps/prefs.xpm"
#include "pixmaps/equalizer.xpm"
#include "pixmaps/playlist.xpm"
#include "pixmaps/skin.xpm"
#include "pixmaps/sound.xpm"
#include "pixmaps/open.xpm"
#include "pixmaps/play.xpm"
#include "pixmaps/stop.xpm"
#include "pixmaps/pause.xpm"
#include "pixmaps/prev.xpm"
#include "pixmaps/next.xpm"
#include "pixmaps/aspect.xpm"
#include "pixmaps/aspect11.xpm"
#include "pixmaps/aspect169.xpm"
#include "pixmaps/aspect235.xpm"
#include "pixmaps/aspect43.xpm"
#include "pixmaps/file2.xpm"
#include "pixmaps/url.xpm"
#include "pixmaps/sub.xpm"
#include "pixmaps/nosub.xpm"
#include "pixmaps/empty.xpm"
#include "pixmaps/loadeaf.xpm"
#include "pixmaps/title.xpm"
#include "pixmaps/subtitle.xpm"
#ifdef CONFIG_CDDA
#include "pixmaps/cd.xpm"
#include "pixmaps/playcd.xpm"
#endif
#ifdef CONFIG_VCD
#include "pixmaps/vcd.xpm"
#include "pixmaps/playvcd.xpm"
#endif
#ifdef CONFIG_DVDREAD
#include "pixmaps/dvd.xpm"
#include "pixmaps/playdvd.xpm"
#include "pixmaps/chapter.xpm"
#include "pixmaps/dolby.xpm"
#endif
#ifdef CONFIG_TV
#include "pixmaps/tv.xpm"
#endif
#if defined(CONFIG_LIBCDIO) || defined(CONFIG_DVDREAD)
#include "pixmaps/playimage.xpm"
#endif
#include "pixmaps/empty1px.xpm"
#include "pixmaps/rotate.xpm"
#include "pixmaps/rotate0.xpm"
#include "pixmaps/rotate180.xpm"
#include "pixmaps/rotate90ccw.xpm"
#include "pixmaps/rotate90cw.xpm"

int gtkPopupMenu;
int gtkPopupMenuParam;

static void ActivateMenuItem( int Item )
{
// fprintf( stderr,"[menu] item: %d.%d\n",Item&0xffff,Item>>16 );
 gtkPopupMenu=Item & 0x0000ffff;
 gtkPopupMenuParam=Item >> 16;
 uiEvent( Item & 0x0000ffff,Item >> 16 );
}

static GtkWidget * AddMenuCheckItem(GtkWidget *window1, const char * immagine_xpm, GtkWidget* Menu,const char* label, gboolean state, int Number)
{
 GtkWidget * Label = NULL;
 GtkWidget * Pixmap = NULL;
 GtkWidget * hbox = NULL;
 GtkWidget * Item = NULL;

 GdkPixmap *PixmapIcon = NULL;
 GdkColor transparent;
 GdkBitmap *MaskIcon = NULL;

 PixmapIcon = gdk_pixmap_create_from_xpm_d (window1->window, &MaskIcon, &transparent,(gchar **)immagine_xpm );
 Pixmap = gtk_pixmap_new (PixmapIcon, MaskIcon);
 gdk_pixmap_unref (PixmapIcon);

 Item=gtk_check_menu_item_new();
 Label = gtk_label_new (label);

 hbox = gtk_hbox_new (FALSE, 8);
 gtk_box_pack_start (GTK_BOX (hbox), Pixmap, FALSE, FALSE, 0);
 gtk_box_pack_start (GTK_BOX (hbox), Label, FALSE, FALSE, 0);
 gtk_container_add (GTK_CONTAINER (Item), hbox);

 gtk_menu_append( GTK_MENU( Menu ),Item );

 gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(Item),state);
 gtk_signal_connect_object( GTK_OBJECT(Item),"activate",
   GTK_SIGNAL_FUNC(ActivateMenuItem),GINT_TO_POINTER(Number) );
 gtk_menu_item_right_justify (GTK_MENU_ITEM (Item));
 gtk_widget_show_all(Item);

 return Item;
}
GtkWidget * AddMenuItem( GtkWidget *window1, const char * immagine_xpm,  GtkWidget * SubMenu,const char * label,int Number )
{
 GtkWidget * Label = NULL;
 GtkWidget * Pixmap = NULL;
 GtkWidget * hbox = NULL;
 GtkWidget * Item = NULL;
 GdkPixmap * PixmapIcon = NULL;
 GdkColor transparent;
 GdkBitmap * MaskIcon = NULL;

 PixmapIcon = gdk_pixmap_create_from_xpm_d (window1->window, &MaskIcon, &transparent,(gchar **)immagine_xpm );
 Pixmap = gtk_pixmap_new (PixmapIcon, MaskIcon);
 gdk_pixmap_unref (PixmapIcon);

 Item=gtk_menu_item_new();
 Label = gtk_label_new (label);

 hbox = gtk_hbox_new (FALSE, 8);
 gtk_box_pack_start (GTK_BOX (hbox), Pixmap, FALSE, FALSE, 0);
 gtk_box_pack_start (GTK_BOX (hbox), Label, FALSE, FALSE, 0);
 gtk_container_add (GTK_CONTAINER (Item), hbox);


 gtk_menu_append( GTK_MENU( SubMenu ),Item );
 gtk_signal_connect_object( GTK_OBJECT(Item),"activate",
   GTK_SIGNAL_FUNC(ActivateMenuItem),GINT_TO_POINTER(Number) );

 gtk_menu_item_right_justify (GTK_MENU_ITEM (Item));
 gtk_widget_show_all(Item);
 return Item;
}


GtkWidget * AddSubMenu( GtkWidget *window1, const char * immagine_xpm, GtkWidget * Menu,const char * label )
{
 GtkWidget * Label = NULL;
 GtkWidget * Pixmap = NULL;
 GtkWidget * hbox = NULL;
 GtkWidget * Item = NULL;
 GtkWidget * SubItem = NULL;
 GdkPixmap * PixmapIcon = NULL;
 GdkColor transparent;
 GdkBitmap * MaskIcon = NULL;

 PixmapIcon = gdk_pixmap_create_from_xpm_d (window1->window, &MaskIcon, &transparent,(gchar **)immagine_xpm);
 Pixmap = gtk_pixmap_new (PixmapIcon, MaskIcon);
 gdk_pixmap_unref (PixmapIcon);

 SubItem=gtk_menu_item_new();
 Item=gtk_menu_new();
 Label = gtk_label_new (label);

 hbox = gtk_hbox_new (FALSE, 8);
 gtk_box_pack_start (GTK_BOX (hbox), Pixmap, FALSE, FALSE, 0);
 gtk_box_pack_start (GTK_BOX (hbox), Label, FALSE, FALSE, 0);
 gtk_container_add (GTK_CONTAINER (SubItem), hbox);

 gtk_menu_append( GTK_MENU( Menu ),SubItem );
 gtk_menu_item_set_submenu( GTK_MENU_ITEM( SubItem ),Item );

 gtk_widget_show_all( SubItem );
 return Item;
}

GtkWidget * AddSeparator( GtkWidget * Menu )
{
 GtkWidget * Item = NULL;

 Item=gtk_menu_item_new ();
 gtk_widget_show( Item );
 gtk_container_add( GTK_CONTAINER( Menu ),Item );
 gtk_widget_set_sensitive( Item,FALSE );

 return Item;
}

typedef struct
{
 int id;
 const char * id2;
 const char * name;
} Languages_t;

#define lng( a,b ) ( (int)(a) * 256 + b )
static Languages_t Languages[] =
{
  { lng( 'a','a' ), "aar", "ʿAfár af"                      },
  { lng( 'a','b' ), "abk", "аҧсуа бызшәа"         },
  { lng( 'a','f' ), "afr", "Afrikaans"                       },
  { lng( 'a','m' ), "amh", "ኣማርኛ"                    },
  { lng( 'a','r' ), "ara", "العربية"                  },
  { lng( 'a','s' ), "asm", "অসমীয়া"           },
  { lng( 'a','y' ), "aym", "Aymar Aru"                       },
  { lng( 'a','z' ), "aze", "Azərbaycanca"                   },
  { lng( 'b','a' ), "bak", "Башҡорт теле"         },
  { lng( 'b','e' ), "bel", "беларуская мова"   },
  { lng( 'b','g' ), "bul", "български език"     },
  { lng( 'b','i' ), "bis", "Bislama"                         },
  { lng( 'b','n' ), "ben", "বাংলা"                 },
  { lng( 'b','o' ), "bod", "བོད་སྐད"           },
  { lng( 'b','o' ), "tib", "བོད་སྐད"           },
  { lng( 'b','r' ), "bre", "Brezhoneg"                       },
  { lng( 'b','s' ), "bos", "Bosanski"                        },
  { lng( 'c','a' ), "cat", "Català"                         },
  { lng( 'c','e' ), "che", "Нохчийн мотт"         },
  { lng( 'c','h' ), "cha", "Chamoru"                         },
  { lng( 'c','o' ), "cos", "Corsu"                           },
  { lng( 'c','s' ), "ces", "Čeština"                       },
  { lng( 'c','s' ), "cze", "Čeština"                       },
  { lng( 'c','v' ), "chv", "Чӑвашла"                  },
  { lng( 'c','y' ), "cym", "Cymraeg"                         },
  { lng( 'c','y' ), "wel", "Cymraeg"                         },
  { lng( 'd','a' ), "dan", "Dansk"                           },
  { lng( 'd','e' ), "deu", "Deutsch"                         },
  { lng( 'd','e' ), "ger", "Deutsch"                         },
  { lng( 'd','z' ), "dzo", "ཇོང་ཁ"                 },
  { lng( 'e','l' ), "ell", "Ελληνικά"                },
  { lng( 'e','l' ), "gre", "Ελληνικά"                },
  { lng( 'e','n' ), "eng", "English"                         },
  { lng( 'e','o' ), "epo", "Esperanto"                       },
  { lng( 'e','s' ), "spa", "Español"                        },
  { lng( 'e','t' ), "est", "Eesti keel"                      },
  { lng( 'e','u' ), "eus", "Euskara"                         },
  { lng( 'e','u' ), "baq", "Euskara"                         },
  { lng( 'f','a' ), "fas", "فارسی"                      },
  { lng( 'f','a' ), "per", "فارسی"                      },
  { lng( 'f','i' ), "fin", "Suomi"                           },
  { lng( 'f','j' ), "fij", "Vakaviti"                        },
  { lng( 'f','o' ), "fao", "Føroyskt"                       },
  { lng( 'f','r' ), "fra", "Français"                       },
  { lng( 'f','r' ), "fre", "Français"                       },
  { lng( 'f','y' ), "fry", "Frysk"                           },
  { lng( 'g','a' ), "gle", "Gaeilge"                         },
  { lng( 'g','d' ), "gla", "Gàidhlig"                       },
  { lng( 'g','l' ), "glg", "Galego"                          },
  { lng( 'g','n' ), "grn", "Avañe'ẽ"                      },
  { lng( 'g','u' ), "guj", "ગુજરાતી"           },
  { lng( 'h','a' ), "hau", "هَوُسَ"                    },
  { lng( 'h','e' ), "heb", "עִבְרִית"                },
  { lng( 'h','i' ), "hin", "हिन्दी"              },
  { lng( 'h','r' ), "hrv", "Hrvatska"                        },
  { lng( 'h','u' ), "hun", "Magyar"                          },
  { lng( 'h','y' ), "hye", "Հայերեն"                  },
  { lng( 'h','y' ), "arm", "Հայերեն"                  },
  { lng( 'h','z' ), "her", "Otjiherero"                      },
  { lng( 'i','d' ), "ind", "Bahasa Indonesia"                },
  { lng( 'i','s' ), "isl", "Íslenska"                       },
  { lng( 'i','s' ), "ice", "Íslenska"                       },
  { lng( 'i','t' ), "ita", "Italiano"                        },
  { lng( 'i','u' ), "iku", "ᐃᓄᒃᑎᑐᑦ"              },
  { lng( 'j','a' ), "jpn", "日本語"                       },
  { lng( 'j','v' ), "jav", "Basa Jawa"                       },
  { lng( 'k','a' ), "kat", "ქართული"           },
  { lng( 'k','a' ), "geo", "ქართული"           },
  { lng( 'k','i' ), "kik", "Gĩkũyũ"                       },
  { lng( 'k','j' ), "kua", "Kuanyama"                        },
  { lng( 'k','k' ), "kaz", "Қазақ тілі"             },
  { lng( 'k','l' ), "kal", "Kalaallisut"                     },
  { lng( 'k','m' ), "khm", "ភាសាខ្មែរ"     },
  { lng( 'k','n' ), "kan", "ಕನ್ನಡ"                 },
  { lng( 'k','o' ), "kor", "한국어"                       },
  { lng( 'k','s' ), "kas", "कॉशुर"                 },
  { lng( 'k','u' ), "kur", "کوردی"                      },
  { lng( 'k','v' ), "kom", "коми кыв"                 },
  { lng( 'k','y' ), "kir", "Кыргызча"                },
  { lng( 'l','a' ), "lat", "Lingua latina"                   },
  { lng( 'l','b' ), "ltz", "Lëtzebuergesch"                 },
  { lng( 'l','n' ), "lin", "Lingála"                        },
  { lng( 'l','o' ), "lao", "ພາສາລາວ"           },
  { lng( 'l','t' ), "lit", "Lietuvių kalba"                 },
  { lng( 'l','v' ), "lav", "Latviešu"                       },
  { lng( 'm','g' ), "mlg", "Malagasy"                        },
  { lng( 'm','h' ), "mah", "Kajin M̧ajeļ"                  },
  { lng( 'm','i' ), "mri", "Te Reo Māori"                   },
  { lng( 'm','i' ), "mao", "Te Reo Māori"                   },
  { lng( 'm','k' ), "mkd", "Македонски јазик" },
  { lng( 'm','k' ), "mac", "Македонски јазик" },
  { lng( 'm','l' ), "mal", "മലയാളം"              },
  { lng( 'm','n' ), "mon", "ᠮᠣᠨᠭᠭᠣᠯ"           },
  { lng( 'm','r' ), "mar", "मराठी"                 },
  { lng( 'm','s' ), "msa", "Bahasa Melayu"                   },
  { lng( 'm','s' ), "may", "Bahasa Melayu"                   },
  { lng( 'm','t' ), "mlt", "Malti"                           },
  { lng( 'm','y' ), "mya", "မြန်မာစကား"  },
  { lng( 'm','y' ), "bur", "မြန်မာစကား"  },
  { lng( 'n','b' ), "nob", "Bokmål"                         },
  { lng( 'n','d' ), "nde", "isiNdebele"                      },
  { lng( 'n','e' ), "nep", "नेपाली"              },
  { lng( 'n','l' ), "nld", "Nederlands"                      },
  { lng( 'n','l' ), "dut", "Nederlands"                      },
  { lng( 'n','n' ), "nno", "Nynorsk"                         },
  { lng( 'n','r' ), "nbl", "isiNdebele"                      },
  { lng( 'n','v' ), "nav", "Diné bizaad"                    },
  { lng( 'n','y' ), "nya", "Chichewa"                        },
  { lng( 'o','c' ), "oci", "Occitan"                         },
  { lng( 'o','m' ), "orm", "Afaan Oromoo"                    },
  { lng( 'o','r' ), "ori", "ଓଡ଼ିଆ"                 },
  { lng( 'o','s' ), "oss", "Ирон ӕвзаг"             },
  { lng( 'p','a' ), "pan", "ਪੰਜਾਬੀ"              },
  { lng( 'p','l' ), "pol", "Język polski"                   },
  { lng( 'p','s' ), "pus", "‏پښتو"                     },
  { lng( 'p','t' ), "por", "Português"                      },
  { lng( 'q','u' ), "que", "Runa Simi"                       },
  { lng( 'r','m' ), "roh", "Rätoromanisch"                  },
  { lng( 'r','n' ), "run", "íkiRǔndi"                      },
  { lng( 'r','o' ), "ron", "Română)"                       },
  { lng( 'r','o' ), "rum", "Română)"                       },
  { lng( 'r','u' ), "rus", "Русский"                  },
  { lng( 'r','w' ), "kin", "Ikinyarwanda"                    },
  { lng( 's','c' ), "srd", "Sardu"                           },
  { lng( 's','d' ), "snd", "‏سنڌي‎"                  },
  { lng( 's','g' ), "sag", "Sängö"                         },
  { lng( 's','i' ), "sin", "සිංහල"                 },
  { lng( 's','k' ), "slk", "Slovenčina"                     },
  { lng( 's','k' ), "slo", "Slovenčina"                     },
  { lng( 's','l' ), "slv", "Slovenščina"                   },
  { lng( 's','m' ), "smo", "Gagana Sāmoa"                   },
  { lng( 's','n' ), "sna", "chiShona"                        },
  { lng( 's','o' ), "som", "Af-ka Soomaali-ga"               },
  { lng( 's','q' ), "sqi", "Shqip"                           },
  { lng( 's','q' ), "alb", "Shqip"                           },
  { lng( 's','r' ), "srp", "Српски"                    },
  { lng( 's','s' ), "ssw", "siSwati"                         },
  { lng( 's','t' ), "sot", "seSotho"                         },
  { lng( 's','u' ), "sun", "Basa Sunda"                      },
  { lng( 's','v' ), "swe", "Svenska"                         },
  { lng( 's','w' ), "swa", "Kiswahili"                       },
  { lng( 't','a' ), "tam", "தமிழ்"                 },
  { lng( 't','e' ), "tel", "తెలుగు"              },
  { lng( 't','g' ), "tgk", "тоҷикӣ"                    },
  { lng( 't','h' ), "tha", "ภาษาไทย"           },
  { lng( 't','i' ), "tir", "ትግርኛ"                    },
  { lng( 't','k' ), "tuk", "Türkmençe"                     },
  { lng( 't','l' ), "tgl", "Tagalog"                         },
  { lng( 't','n' ), "tsn", "Setswana"                        },
  { lng( 't','o' ), "ton", "Lea fakatonga"                   },
  { lng( 't','r' ), "tur", "Türkçe"                        },
  { lng( 't','s' ), "tso", "Xitsonga"                        },
  { lng( 't','t' ), "tat", "татарча"                  },
  { lng( 't','w' ), "twi", "Twi"                             },
  { lng( 't','y' ), "tah", "Reo Tahiti"                      },
  { lng( 'u','g' ), "uig", "ئۇيغۇرچە‎"             },
  { lng( 'u','k' ), "ukr", "Українська"            },
  { lng( 'u','r' ), "urd", "‏اردو‎"                  },
  { lng( 'u','z' ), "uzb", "Oʻzbek tili"                    },
  { lng( 'v','i' ), "vie", "Tiếng Việt"                  },
  { lng( 'w','o' ), "wol", "Wolof"                           },
  { lng( 'x','h' ), "xho", "isiXhosa"                        },
  { lng( 'y','i' ), "yid", "יידיש"                      },
  { lng( 'y','o' ), "yor", "Yorùbá"                        },
  { lng( 'z','a' ), "zha", "壮语"                          },
  { lng( 'z','h' ), "zho", "漢語"                          },
  { lng( 'z','h' ), "chi", "漢語"                          },
  { lng( 'z','u' ), "zul", "isiZulu"                         },
};

#ifdef CONFIG_DVDREAD
static char * ChannelTypes[] =
  { "Dolby Digital","","Mpeg1","Mpeg2","PCM","","Digital Theatre System" };
static char * ChannelNumbers[] =
  { "","Stereo","","","","5.1" };
#endif

enum
{
  GET_LANG_INT,
  GET_LANG_CHR
};

static const char * GetLanguage( void *language, int type )
{
 int l;
 char *p;
 unsigned int i;
 if ( type == GET_LANG_INT ) l = *(int *) language;
 else
  {
    p = language;

    if ( p[2] == 0)
     {
       l = lng( p[0], p[1] );
       type = GET_LANG_INT;
     }
    else if ( p[3] != 0) return language;
  }
 for ( i=0;i<sizeof( Languages ) / sizeof( Languages_t );i++ )
  if ( type == GET_LANG_INT ? Languages[i].id == l : strcasecmp(Languages[i].id2, p) == 0 ) return Languages[i].name;
 return MSGTR_GUI_Unknown;
}
#undef lng


#ifdef CONFIG_DVDREAD
static GtkWidget * DVDSubMenu;
#endif
GtkWidget * DVDTitleMenu;
GtkWidget * DVDChapterMenu;
GtkWidget * DVDAudioLanguageMenu;
GtkWidget * DVDSubtitleLanguageMenu;
GtkWidget * AspectMenu;
GtkWidget * RotationMenu;
GtkWidget * VCDSubMenu;
GtkWidget * VCDTitleMenu;
GtkWidget * CDSubMenu;
GtkWidget * CDTitleMenu;

GtkWidget * CreatePopUpMenu( void )
{
 GtkWidget * window1;
 GtkWidget * Menu = NULL;
 GtkWidget * SubMenu = NULL;
 GtkWidget * MenuItem = NULL;
 GtkWidget * H, * N, * D, * F;
 demuxer_t *demuxer = mpctx_get_demuxer(guiInfo.mpcontext);
 mixer_t *mixer = mpctx_get_mixer(guiInfo.mpcontext);
 int subs = 0, sub_pos;

 Menu=gtk_menu_new();
 gtk_widget_realize (Menu);
 window1 = gtk_widget_get_toplevel(Menu);


  AddMenuItem( window1, (const char*)about_xpm, Menu,MSGTR_GUI_AboutMPlayer"     ", evAbout );
  AddSeparator( Menu );
   SubMenu=AddSubMenu( window1, (const char*)open_xpm, Menu,MSGTR_GUI_Open );
    AddMenuItem( window1, (const char*)file2_xpm, SubMenu,MSGTR_GUI_File"...    ", evLoadPlay );
#ifdef CONFIG_CDDA
    AddMenuItem( window1, (const char*)playcd_xpm, SubMenu,MSGTR_GUI_CD, evPlayCD );
    CDSubMenu=AddSubMenu( window1, (const char*)cd_xpm, Menu,MSGTR_GUI_CD );
    AddMenuItem( window1, (const char*)playcd_xpm, CDSubMenu,MSGTR_GUI_Play,evPlayCD );
    AddSeparator( CDSubMenu );
    CDTitleMenu=AddSubMenu( window1, (const char*)title_xpm, CDSubMenu,MSGTR_GUI_Titles );
    if ( guiInfo.Tracks && ( guiInfo.StreamType == STREAMTYPE_CDDA ) )
     {
      char tmp[32]; int i;
      for ( i=1;i <= guiInfo.Tracks;i++ )
       {
        snprintf( tmp,32,MSGTR_GUI_TitleNN,i );
    //AddMenuItem( CDTitleMenu,tmp,( i << 16 ) + ivSetCDTrack );
        AddMenuCheckItem(window1, (const char*)empty1px_xpm, CDTitleMenu,tmp, guiInfo.Track == i, ( i << 16 ) + ivSetCDTrack );
       }
     }
     else
      {
       MenuItem=AddMenuItem( window1, (const char*)empty1px_xpm, CDTitleMenu,MSGTR_GUI__none_,evNone );
       gtk_widget_set_sensitive( MenuItem,FALSE );
      }
#endif
#ifdef CONFIG_VCD
    AddMenuItem( window1, (const char*)playvcd_xpm, SubMenu,MSGTR_GUI_VCD, evPlayVCD );
    VCDSubMenu=AddSubMenu( window1, (const char*)vcd_xpm, Menu,MSGTR_GUI_VCD );
    AddMenuItem( window1, (const char*)playvcd_xpm, VCDSubMenu,MSGTR_GUI_Play,evPlayVCD );
    AddSeparator( VCDSubMenu );
    VCDTitleMenu=AddSubMenu( window1, (const char*)title_xpm, VCDSubMenu,MSGTR_GUI_Titles );
    if ( guiInfo.Tracks && ( guiInfo.StreamType == STREAMTYPE_VCD ) )
     {
      char tmp[32]; int i;
      for ( i=1;i < guiInfo.Tracks;i++ )
       {
        snprintf( tmp,32,MSGTR_GUI_TitleNN,i );
    //AddMenuItem( VCDTitleMenu,tmp,( i << 16 ) + ivSetVCDTrack );
        AddMenuCheckItem(window1, (const char*)empty1px_xpm, VCDTitleMenu,tmp, guiInfo.Track == i + 1, ( ( i + 1 ) << 16 ) + ivSetVCDTrack );
       }
     }
     else
      {
       MenuItem=AddMenuItem( window1, (const char*)empty1px_xpm, VCDTitleMenu,MSGTR_GUI__none_,evNone );
       gtk_widget_set_sensitive( MenuItem,FALSE );
      }
#endif
#ifdef CONFIG_DVDREAD
    AddMenuItem( window1, (const char*)playdvd_xpm, SubMenu,MSGTR_GUI_DVD, evPlayDVD );
    DVDSubMenu=AddSubMenu( window1, (const char*)dvd_xpm, Menu,MSGTR_GUI_DVD );
    AddMenuItem( window1, (const char*)playdvd_xpm, DVDSubMenu,MSGTR_GUI_Play"    ", evPlayDVD );
//    AddMenuItem( DVDSubMenu,MSGTR_MENU_ShowDVDMenu, evNone );
    AddSeparator( DVDSubMenu );
    DVDTitleMenu=AddSubMenu( window1, (const char*)title_xpm, DVDSubMenu,MSGTR_GUI_Titles );
     if ( guiInfo.Tracks && ( guiInfo.StreamType == STREAMTYPE_DVD ) )
      {
       char tmp[32]; int i;
       for ( i=1 ; i<= guiInfo.Tracks;i++ )
        {
         snprintf( tmp,32,MSGTR_GUI_TitleNN,i);
         AddMenuCheckItem( window1, (const char*)empty1px_xpm, DVDTitleMenu,tmp,
         guiInfo.Track == i,
         (i << 16) + ivSetDVDTitle );
        }
      }
      else
       {
        MenuItem=AddMenuItem( window1, (const char*)empty1px_xpm, DVDTitleMenu,MSGTR_GUI__none_,evNone );
        gtk_widget_set_sensitive( MenuItem,FALSE );
       }
    DVDChapterMenu=AddSubMenu( window1, (const char*)chapter_xpm, DVDSubMenu,MSGTR_GUI_Chapters );
     if ( guiInfo.Chapters && ( guiInfo.StreamType == STREAMTYPE_DVD ) )
      {
       char tmp[32]; int i;
       for ( i=1;i <= guiInfo.Chapters;i++ )
        {
         snprintf( tmp,32,MSGTR_GUI_ChapterNN,i );
         AddMenuCheckItem( window1, (const char*)empty1px_xpm, DVDChapterMenu,tmp,guiInfo.Chapter == i,
         ( i << 16 ) + ivSetDVDChapter );
        }
      }
      else
       {
        MenuItem=AddMenuItem( window1, (const char*)empty1px_xpm, DVDChapterMenu,MSGTR_GUI__none_,evNone );
        gtk_widget_set_sensitive( MenuItem,FALSE );
       }
    DVDAudioLanguageMenu=AddSubMenu( window1, (const char*)audio_xpm, DVDSubMenu,MSGTR_GUI_AudioTracks );
     if ( guiInfo.AudioStreams && demuxer && ( guiInfo.StreamType == STREAMTYPE_DVD ) )
      {
       char tmp[64]; int i;
       for ( i=0;i < guiInfo.AudioStreams;i++ )
        {
   snprintf( tmp,64,"%s - %s %s",GetLanguage( &guiInfo.AudioStream[i].language, GET_LANG_INT ),
     ChannelTypes[ guiInfo.AudioStream[i].type ],
     ChannelNumbers[ guiInfo.AudioStream[i].channels ] );
         AddMenuCheckItem( window1, (const char*)dolby_xpm, DVDAudioLanguageMenu,tmp,
         demuxer->audio->id == guiInfo.AudioStream[i].id,
         ( guiInfo.AudioStream[i].id << 16 ) + ivSetDVDAudio );
        }
      }
      else
       {
        MenuItem=AddMenuItem( window1, (const char*)empty1px_xpm, DVDAudioLanguageMenu,MSGTR_GUI__none_,evNone );
        gtk_widget_set_sensitive( MenuItem,FALSE );
       }
    DVDSubtitleLanguageMenu=AddSubMenu( window1, (const char*)subtitle_xpm, DVDSubMenu,MSGTR_GUI_Subtitles );
     if ( guiInfo.Subtitles && ( guiInfo.StreamType == STREAMTYPE_DVD ) )
      {
       char tmp[64]; int i;
       AddMenuItem( window1, (const char*)empty1px_xpm, DVDSubtitleLanguageMenu,MSGTR_GUI__none_,( (unsigned short)-1 << 16 ) + ivSetDVDSubtitle );
       for ( i=0;i < guiInfo.Subtitles;i++ )
        {
         av_strlcpy( tmp,GetLanguage( &guiInfo.Subtitle[i].language, GET_LANG_INT ),sizeof(tmp) );
         AddMenuCheckItem( window1, (const char*)empty1px_xpm, DVDSubtitleLanguageMenu,tmp,
         dvdsub_id == guiInfo.Subtitle[i].id,
         ( guiInfo.Subtitle[i].id << 16 ) + ivSetDVDSubtitle );
        }
      }
      else
       {
        MenuItem=AddMenuItem( window1, (const char*)empty1px_xpm, DVDSubtitleLanguageMenu,MSGTR_GUI__none_,evNone );
        gtk_widget_set_sensitive( MenuItem,FALSE );
       }
#endif
#if defined(CONFIG_LIBCDIO) || defined(CONFIG_DVDREAD)
    AddMenuItem( window1, (const char*)playimage_xpm, SubMenu,MSGTR_GUI_Image"...    ", evPlayImage );
#endif
    AddMenuItem( window1, (const char*)url_xpm, SubMenu,MSGTR_GUI_URL"...", evLoadURL );
#ifdef CONFIG_TV
    AddMenuItem( window1, (const char*)tv_xpm, SubMenu,MSGTR_GUI_TV, evPlayTV );
#endif
    AddSeparator( SubMenu );
    AddMenuItem( window1, (const char*)loadeaf_xpm, SubMenu,MSGTR_GUI_AudioTrack"...", evLoadAudioFile );
    AddMenuItem( window1, (const char*)sub_xpm, SubMenu,MSGTR_GUI_Subtitle"...   ", evLoadSubtitle );
    AddMenuItem( window1, (const char*)nosub_xpm, SubMenu,MSGTR_GUI_DropSubtitle,evDropSubtitle );
   SubMenu=AddSubMenu(window1, (const char*)play_xpm, Menu,MSGTR_GUI_Playback );
    AddMenuItem( window1, (const char*)play_xpm, SubMenu,MSGTR_GUI_Play"        ", evPlay );
    AddMenuItem( window1, (const char*)pause_xpm, SubMenu,MSGTR_GUI_Pause, evPause );
    AddMenuItem( window1, (const char*)stop_xpm, SubMenu,MSGTR_GUI_Stop, evStop );
    AddMenuItem( window1, (const char*)next_xpm, SubMenu,MSGTR_GUI_Next, evNext );
    AddMenuItem( window1, (const char*)prev_xpm, SubMenu,MSGTR_GUI_Previous, evPrev );
//    AddSeparator( SubMenu );
//    AddMenuItem( SubMenu,"Back 10 sec", evBackward10sec );
//    AddMenuItem( SubMenu,"Fwd 10 sec", evForward10sec );
//    AddMenuItem( SubMenu,"Back 1 min", evBackward1min );
//    AddMenuItem( SubMenu,"Fwd 1 min", evForward1min );
//   SubMenu=AddSubMenu( Menu,MSGTR_MENU_Size );
//    AddMenuItem( SubMenu,MSGTR_GUI_SizeNormal"      ", evNormalSize );
//    AddMenuItem( SubMenu,MSGTR_GUI_SizeDouble, evDoubleSize );
//    AddMenuItem( SubMenu,MSGTR_GUI_SizeFullscreen, evFullScreen + ( True << 16 ) );

  if ( guiInfo.VideoWindow )
   {
    AddSeparator( Menu );
    RotationMenu=AddSubMenu( window1, (const char*)rotate_xpm, Menu,MSGTR_GUI_Rotation );
    N=AddMenuCheckItem( window1, (const char*)rotate0_xpm, RotationMenu,MSGTR_GUI__none_, guiInfo.Rotation == -1, evSetRotation );
    D=AddMenuCheckItem( window1, (const char*)rotate90cw_xpm, RotationMenu,MSGTR_GUI_Rotation90CW, guiInfo.Rotation == 1, evSetRotation + ( 90 << 16 ) );
    F=AddMenuCheckItem( window1, (const char*)rotate90ccw_xpm, RotationMenu,MSGTR_GUI_Rotation90CCW, guiInfo.Rotation == 2, evSetRotation + ( -90 << 16 ) );
    H=AddMenuCheckItem( window1, (const char*)rotate180_xpm, RotationMenu,MSGTR_GUI_Rotation180, guiInfo.Rotation == 8, evSetRotation + ( 180 << 16 ) );

    if ( !guiInfo.Playing )
     {
      gtk_widget_set_sensitive( N,FALSE );
      gtk_widget_set_sensitive( D,FALSE );
      gtk_widget_set_sensitive( F,FALSE );
      gtk_widget_set_sensitive( H,FALSE );
     }
   }

  if ( guiInfo.VideoWindow )
   {
    int a11 = False, a169 = False, a43 = False, a235 = False;

    if (movie_aspect == -1.0f) a11 = True;
    else
     {
       a169 = (FFABS(movie_aspect - 16.0f / 9.0f) <= 0.01f);
       a43 = (FFABS(movie_aspect - 4.0f / 3.0f) <= 0.01f);
       a235 = (FFABS(movie_aspect - 2.35f) <= 0.01f);
     }

    AspectMenu=AddSubMenu( window1, (const char*)aspect_xpm, Menu,MSGTR_GUI_AspectRatio );
    H=AddMenuCheckItem( window1, (const char*)aspect11_xpm, AspectMenu,MSGTR_GUI_Original, a11, evSetAspect + ( 1 << 16 ) );
    N=AddMenuCheckItem( window1, (const char*)aspect169_xpm, AspectMenu,"16:9", a169, evSetAspect + ( 2 << 16 ) );
    D=AddMenuCheckItem( window1, (const char*)aspect43_xpm, AspectMenu,"4:3", a43, evSetAspect + ( 3 << 16 ) );
    F=AddMenuCheckItem( window1, (const char*)aspect235_xpm, AspectMenu,MSGTR_GUI_235To1, a235, evSetAspect + ( 4 << 16 ) );

    if ( !guiInfo.Playing )
     {
      gtk_widget_set_sensitive( H,FALSE );
      gtk_widget_set_sensitive( N,FALSE );
      gtk_widget_set_sensitive( D,FALSE );
      gtk_widget_set_sensitive( F,FALSE );
     }
   }

  if ( guiInfo.VideoWindow )
   {
    int b1 = False, b2 = False, b_half = False;

    if ( !guiApp.videoWindow.isFullScreen && guiInfo.Playing )
     {
      if ( ( guiApp.videoWindow.Width == guiInfo.VideoWidth * 2 )&&
           ( guiApp.videoWindow.Height == guiInfo.VideoHeight * 2 ) ) b2=True;
      else if ( ( guiApp.videoWindow.Width == guiInfo.VideoWidth / 2 ) &&
                ( guiApp.videoWindow.Height == guiInfo.VideoHeight / 2 ) ) b_half=True;
      else b1=( guiApp.videoWindow.Width == guiInfo.VideoWidth && guiApp.videoWindow.Height == guiInfo.VideoHeight );
     } else b1=!guiApp.videoWindow.isFullScreen;
    F=AddMenuCheckItem( window1, (const char*)full_xpm, Menu,MSGTR_GUI_SizeFullscreen,guiApp.videoWindow.isFullScreen,evFullScreen + ( True << 16 ) );
    D=AddMenuCheckItem( window1, (const char*)double_xpm, Menu,MSGTR_GUI_SizeDouble,b2,evDoubleSize );
    N=AddMenuCheckItem( window1, (const char*)normal_xpm, Menu,MSGTR_GUI_SizeNormal"      ",b1,evNormalSize );
    H=AddMenuCheckItem( window1, (const char*)half_xpm, Menu,MSGTR_GUI_SizeHalf,b_half,evHalfSize );
    if ( !guiInfo.Playing )
     {
      gtk_widget_set_sensitive( H,FALSE );
      gtk_widget_set_sensitive( N,FALSE );
      gtk_widget_set_sensitive( D,FALSE );
      gtk_widget_set_sensitive( F,FALSE );
     }
   }

  AddSeparator( Menu );
  MenuItem=AddMenuCheckItem( window1, (const char*)sound_xpm, Menu,MSGTR_GUI_Mute,mixer->muted,evMute );
  if ( !guiInfo.AudioChannels ) gtk_widget_set_sensitive( MenuItem,FALSE );

  if ( guiInfo.Playing && demuxer && guiInfo.StreamType != STREAMTYPE_DVD )
   {
    int i,c = 0;

    for ( i=0;i < MAX_A_STREAMS;i++ )
     if ( demuxer->a_streams[i] ) c++;

    if ( c > 1 )
     {
      SubMenu=AddSubMenu( window1, (const char*)audio_xpm, Menu,MSGTR_GUI_AudioTracks );
      for ( i=0;i < MAX_A_STREAMS;i++ )
       if ( demuxer->a_streams[i] )
        {
         int aid = ((sh_audio_t *)demuxer->a_streams[i])->aid;
         int selected_id = (audio_id == aid || (audio_id == -1 && aid == demuxer_default_audio_track(demuxer)));
         char lng[32], tmp[64];
         if ( demuxer_audio_lang( demuxer, i, lng, sizeof(lng) ) == 0 ) snprintf( tmp,sizeof(tmp),MSGTR_GUI_TrackN" - %s",aid,GetLanguage( lng, GET_LANG_CHR ) );
         else snprintf( tmp,sizeof(tmp),MSGTR_GUI_TrackN,aid );
         AddMenuCheckItem( window1, (const char*)empty1px_xpm, SubMenu,tmp,selected_id,( aid << 16 ) + ivSetAudio );
        }
     }

    for ( c=0,i=0;i < MAX_V_STREAMS;i++ )
     if ( demuxer->v_streams[i] ) c++;

    if ( c > 1 )
     {
      SubMenu=AddSubMenu( window1, (const char*)video_xpm, Menu,MSGTR_GUI_VideoTracks );
      for ( i=0;i < MAX_V_STREAMS;i++ )
       if ( demuxer->v_streams[i] )
        {
         int vid = ((sh_video_t *)demuxer->v_streams[i])->vid;
         char tmp[32];
         int selected_id = (video_id == vid || (video_id == -1 && vid == demuxer_default_video_track(demuxer)));
         snprintf( tmp,32,MSGTR_GUI_TrackN,vid );
         AddMenuCheckItem( window1, (const char*)empty1px_xpm, SubMenu,tmp,selected_id,( vid << 16 ) + ivSetVideo );
        }
     }
   }

  /* cheap subtitle switching for non-DVD streams */

  mpctx_get_global_sub_info(guiInfo.mpcontext, &subs, &sub_pos);

  if ( subs && guiInfo.StreamType != STREAMTYPE_DVD )
   {
    int i, j, subs0 = guiInfo.mpcontext->sub_counts[SUB_SOURCE_SUBS], subs1 = guiInfo.mpcontext->sub_counts[SUB_SOURCE_VOBSUB];
    SubMenu=AddSubMenu( window1, (const char*)subtitle_xpm, Menu, MSGTR_GUI_Subtitles );
    AddMenuCheckItem( window1, (const char*)empty1px_xpm, SubMenu, MSGTR_GUI__none_, sub_pos == -1, (-1 << 16) + ivSetSubtitle );
    for ( i=0;i < subs;i++ )
     {
      int ret = -1;
      char lng[32], tmp[64], *lang = NULL;
      /* file */
      if ( i < subs0 )
       {
        if ( guiInfo.mpcontext->set_of_subtitles[i] ) lang = guiInfo.mpcontext->set_of_subtitles[i]->filename;
#ifdef CONFIG_ASS
        if ( ass_track && ass_track->name ) lang = ass_track->name;
#endif
        if ( lang )
         {
          av_strlcpy( lng, mp_basename(lang), sizeof(lng) );
          ret = 0;
         }
       }
      /* VOBsub */
      else if ( ( i >= subs0 && i < subs0 + subs1 ) && vo_vobsub )
       {
        lang = vobsub_get_id( vo_vobsub, vobsub_get_id_by_index( vo_vobsub, i - subs0 ) );

        if ( lang )
         {
          av_strlcpy( lng, lang, sizeof(lng) );
          ret = 0;
         }
       }
      /* embedded (demuxer) */
      else if ( ( i >= subs0 + subs1 ) && demuxer )
       {
        for ( j=0;j < MAX_S_STREAMS;j++ )
         {
          if ( demuxer->s_streams[j] ) ret++;
          if ( ret == i - subs0 - subs1 )
          {
           ret = demuxer_sub_lang( demuxer, j, lng, sizeof(lng) );
           break;
          }
         }
       }
      if ( ret == 0 ) snprintf( tmp, sizeof(tmp), MSGTR_GUI_TrackN" - %s", i, GetLanguage( lng, GET_LANG_CHR ) );
      else snprintf( tmp, sizeof(tmp), MSGTR_GUI_TrackN, i );
      AddMenuCheckItem( window1,(const char*)empty1px_xpm,SubMenu,tmp,sub_pos == i,( i << 16 ) + ivSetSubtitle );
     }
   }

  AddSeparator( Menu );
  AddMenuItem( window1, (const char*)equalizer_xpm, Menu,MSGTR_GUI_Equalizer, evEqualizer );
  AddMenuItem( window1, (const char*)playlist_xpm, Menu,MSGTR_GUI_Playlist, evPlaylist );
  AddMenuItem( window1, (const char*)skin_xpm, Menu,MSGTR_GUI_SkinBrowser, evSkinBrowser );
  AddMenuItem( window1, (const char*)prefs_xpm, Menu,MSGTR_GUI_Preferences, evPreferences );

  AddSeparator( Menu );
  AddMenuItem( window1, (const char*)exit_xpm, Menu,MSGTR_GUI_Quit, evExit );

 return Menu;
}
