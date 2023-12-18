#ifndef GTKUTIL_H
#define GTKUTIL_H

#include <gtk/gtk.h>
#include "client.h"

extern int client_sock;
extern char name_buf[10];

typedef struct
{
    GtkWidget *entry, *textview;
} Widgets;

typedef struct {
    GtkWidget *id_entry;
} LoginWidgets;
/* GtkEntry 로부터 GtkTextView 로 텍스트 삽입 */
static void insert_text_send(GtkButton *button, Widgets *w);

void* insert_text_received();

// 파일 선택 대화상자를 열어서 선택된 파일의 경로를 TextView에 추가하는 함수
void show_file_chooser(GtkWidget *widget, gpointer data) ;
// 새로운 윈도우를 띄우는 함수
void show_new_window(GtkWidget *widget, const char *id);
//로그인창 로그인 버튼 클릭
void login_button_clicked(GtkWidget *widget, gpointer data);

#endif