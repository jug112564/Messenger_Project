#include "gtkutil.h"

Widgets *w;

/* GtkEntry 로부터 GtkTextView 로 텍스트 삽입 */
static void insert_text_send(GtkButton *button, Widgets *w)
{
    GtkTextBuffer *buffer;
    GtkTextMark *mark;
    GtkTextIter iter;
    const gchar *text;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->textview));
    text = gtk_entry_get_text(GTK_ENTRY(w->entry));
    mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, mark);
    /* 버퍼에 텍스트가 있으면 뉴라인 삽입 */
    if (gtk_text_buffer_get_char_count(buffer))
        gtk_text_buffer_insert(buffer, &iter, "\n", 1);
    gtk_text_buffer_insert(buffer, &iter, text, -1);
    mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(w->textview), mark);

    //메시지 전송
    char *char_text = g_strdup(text);
    send_msg(char_text);
    g_free(char_text);
}

void* insert_text_received()
{
    char buf[MAX_COMBINED_BUF_SIZE];
    int recv_len;
    GtkTextBuffer *buffer;
    GtkTextMark *mark;
    GtkTextIter iter;

    while (1) {
        // recv
        memset(buf, 0, MAX_COMBINED_BUF_SIZE);
        recv_len = recv(client_sock, buf, MAX_COMBINED_BUF_SIZE, 0);

        // error or server closed
        if (recv_len <= 0) {
            if (recv_len == 0) {
                printf("Server closed the connection\n");
            } else {
                perror("recv");
            }
            close(client_sock);
            exit(EXIT_SUCCESS);
        }

        // print buf
        printf("Received: %s\n", buf);

        // insert received text into GTK text view
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->textview));
        mark = gtk_text_buffer_get_insert(buffer);
        gtk_text_buffer_get_iter_at_mark(buffer, &iter, mark);

        /* 버퍼에 텍스트가 있으면 뉴라인 삽입 */
        if (gtk_text_buffer_get_char_count(buffer))
            gtk_text_buffer_insert(buffer, &iter, "\n", 1);
        
        // insert received text
        gtk_text_buffer_insert(buffer, &iter, buf, -1);
        
        mark = gtk_text_buffer_get_insert(buffer);
        gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(w->textview), mark);
    }

}

// 파일 선택 대화상자를 열어서 선택된 파일의 경로를 TextView에 추가하는 함수
void show_file_chooser(GtkWidget *widget, gpointer data) {
    Widgets *w = (Widgets *)data;

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    // 부모 윈도우 가져오기
    GtkWidget *parent_window = gtk_widget_get_toplevel(w->textview);

    // 파일 선택 대화상자 생성
    dialog = gtk_file_chooser_dialog_new("Open File",
                                          GTK_WINDOW(parent_window),
                                          action,
                                          "_Cancel",
                                          GTK_RESPONSE_CANCEL,
                                          "_Open",
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    // 대화상자 실행
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        // 사용자가 파일을 선택했을 때의 처리
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->textview));
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, filename, -1);
        gtk_text_buffer_insert(buffer, &iter, "\n", -1);
        
        // 대화상자 닫기
        gtk_widget_destroy(dialog);
        send_file(filename);
        g_free(filename); 
    }
    else
    {
        // 대화상자 닫기
        gtk_widget_destroy(dialog);
    }

    
}

// 새로운 윈도우를 띄우는 함수
void show_new_window(GtkWidget *widget, const char *id) {
    GtkWidget *new_window, *scrolled_win, *hbox, *vbox,
        *insert, *file;

    w = g_slice_new(Widgets);

    char loginId[50];
    sprintf(loginId, "%s : login success", id);
    

    // 새로운 윈도우 생성
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Welcome");
    gtk_container_set_border_width(GTK_CONTAINER(new_window), 10);

    // 윈도우 크기 설정
    gtk_widget_set_size_request(new_window, 200, 400);

    w->textview = gtk_text_view_new();
    w->entry = gtk_entry_new();
    //파일 추가 버튼
    file = gtk_button_new_with_label("file");
    g_signal_connect(file, "clicked", G_CALLBACK(show_file_chooser), w);
    //insert 버튼
    insert = gtk_button_new_with_label("Send Message");
    g_signal_connect(insert, "clicked", G_CALLBACK(insert_text_send), (gpointer)w);
    scrolled_win = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_win),
                      w->textview);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), w->entry, FALSE,
                       FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), insert, FALSE,
                       FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), file, FALSE,
                       FALSE, 5);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_win,
                       TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE,
                       TRUE, 0);
    gtk_container_add(GTK_CONTAINER(new_window), vbox);

    // user is logined 텍스트 추가
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->textview));
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, loginId, -1);
    gtk_text_buffer_insert(buffer, &iter, "\n", -1);

    // 윈도우를 보이게 함
    gtk_widget_show_all(new_window);
}

//로그인창 로그인 버튼 클릭
void login_button_clicked(GtkWidget *widget, gpointer data) {
    LoginWidgets *login_widgets = (LoginWidgets *)data;
    const char *id = gtk_entry_get_text(GTK_ENTRY(login_widgets->id_entry));


    if (id == NULL || strlen(id) == 0) {
        // ID가 입력되지 않았을 때 처리
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                                         GTK_MESSAGE_ERROR,
                                                         GTK_BUTTONS_OK,
                                                         "Please enter your ID.");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;  // ID가 비어 있으면 로그인 거부
    }
    else if (strlen(id) > 9) {
        // ID가 9글자 이상일때 처리
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                                         GTK_MESSAGE_ERROR,
                                                         GTK_BUTTONS_OK,
                                                         "ID count must be under 9.");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;  // 로그인 거부
    }
    else
    {
        //서버 연결
        strcpy(name_buf, id);
        connect_server("192.168.0.28",8000);

        GThread *recv_thread = g_thread_new("reccv_thread", insert_text_received, NULL);
        
        //새창 띄우기
        show_new_window(widget, id);
    }
}
