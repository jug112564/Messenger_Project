#include <gtk/gtk.h>
#include <stdio.h>
#include "client.h"
#include "gtkutil.h"

int client_sock;
int file_sock;
char name_buf[MAX_NAME_BUF_SIZE];


int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *id_entry;
    GtkWidget *login_button;

    //서버 관련 동작
    init_socket(); //소켓 초기화

    gtk_init(&argc, &argv);

    LoginWidgets *login_widgets = g_slice_new(LoginWidgets);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Login");
    gtk_widget_set_size_request(window, 200, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    id_entry = gtk_entry_new();
    login_button = gtk_button_new_with_label("Login");

    // 로그인 버튼 클릭 시 동작을 설정
    g_signal_connect(login_button, "clicked", G_CALLBACK(login_button_clicked), login_widgets);

    // UI 레이아웃 설정
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), id_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), login_button, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), box);

    // 구조체에 위젯 저장
    login_widgets->id_entry = id_entry;

    gtk_widget_show_all(window);

    gtk_main();

    // 구조체 메모리 해제
    g_slice_free(LoginWidgets, login_widgets);

    return 0;
}