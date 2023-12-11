#include <gtk/gtk.h>

typedef struct
{
    GtkWidget *entry, *textview;
} Widgets;

typedef struct {
    GtkWidget *id_entry;
    GtkWidget *pwd_entry;
} LoginWidgets;

/* GtkEntry 로부터 GtkTextView 로 텍스트 삽입 */
static void insert_text(GtkButton *button, Widgets *w)
{
    GtkTextBuffer *buffer;
    GtkTextMark *mark;
    GtkTextIter iter;
    const gchar *text;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->textview));
    text = gtk_entry_get_text(GTK_ENTRY(w->entry));
    mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_buffer_get_iter_at_mark(buffer, &iter,
                                     mark);
    /* 버퍼에 텍스트가 있으면 뉴라인 삽입 */
    if (gtk_text_buffer_get_char_count(buffer))
        gtk_text_buffer_insert(buffer, &iter, "\n", 1);
    gtk_text_buffer_insert(buffer, &iter, text, -1);
    mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(w->textview), mark);
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
        g_free(filename);
    }

    // 대화상자 닫기
    gtk_widget_destroy(dialog);
}

// 새로운 윈도우를 띄우는 함수
void show_new_window(GtkWidget *widget, gpointer data) {
    GtkWidget *new_window, *scrolled_win, *hbox, *vbox,
        *insert, *file;
    Widgets *w = g_slice_new(Widgets);


    // 새로운 윈도우 생성
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Welcome");
    gtk_container_set_border_width(GTK_CONTAINER(new_window), 10);

    // 윈도우 크기 설정
    gtk_widget_set_size_request(new_window, 200, 100);

    w->textview = gtk_text_view_new();
    w->entry = gtk_entry_new();
    //파일 추가 버튼
    file = gtk_button_new_with_label("file");
    g_signal_connect(file, "clicked", G_CALLBACK(show_file_chooser), w);
    //insert 버튼
    insert = gtk_button_new_with_label("Insert Text");
    g_signal_connect(insert, "clicked",
                     G_CALLBACK(insert_text), (gpointer)w);
    scrolled_win = gtk_scrolled_window_new(NULL,
                                           NULL);
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

    // 윈도우를 보이게 함
    gtk_widget_show_all(new_window);
}



// 로그인 버튼을 눌렀을 때 호출되는 함수
// void on_login_button_clicked(GtkWidget *widget, gpointer data) {
//     // 여기에 실제 로그인 처리 로직을 추가할 수 있음

//     // 로그인 후 새로운 윈도우를 띄움
//     show_new_window(widget, data);
// }

void login_button_clicked(GtkWidget *widget, gpointer data) {
    LoginWidgets *login_widgets = (LoginWidgets *)data;
    const char *id = gtk_entry_get_text(GTK_ENTRY(login_widgets->id_entry));
    const char *pwd = gtk_entry_get_text(GTK_ENTRY(login_widgets->pwd_entry));

    // 간단한 로그인 체크
    if (strcmp(id, "test") == 0 && strcmp(pwd, "1234") == 0) {
        // 로그인 성공 시 새로운 창 열기
        show_new_window(widget, NULL);
    } else {
        // 로그인 실패 메시지 표시
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                                         GTK_MESSAGE_ERROR,
                                                         GTK_BUTTONS_OK,
                                                         "Login failed. Please check your ID and password.");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *id_entry;
    GtkWidget *pwd_entry;
    GtkWidget *login_button;

    gtk_init(&argc, &argv);

    LoginWidgets *login_widgets = g_slice_new(LoginWidgets);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Login Example");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    id_entry = gtk_entry_new();
    pwd_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(pwd_entry), FALSE); // 비밀번호 입력을 위해 가려주기
    login_button = gtk_button_new_with_label("Login");

    // 로그인 버튼 클릭 시 동작을 설정
    g_signal_connect(login_button, "clicked", G_CALLBACK(login_button_clicked), login_widgets);

    // UI 레이아웃 설정
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), id_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), pwd_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), login_button, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), box);

    // 구조체에 위젯 저장
    login_widgets->id_entry = id_entry;
    login_widgets->pwd_entry = pwd_entry;

    gtk_widget_show_all(window);

    gtk_main();

    // 구조체 메모리 해제
    g_slice_free(LoginWidgets, login_widgets);

    return 0;
}

// int main(int argc, char *argv[]) {
//     GtkWidget *window;
//     GtkWidget *grid;
//     GtkWidget *login_button;

//     // GTK 초기화
//     gtk_init(&argc, &argv);

//     // 메인 윈도우 생성
//     window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Login Window");
//     g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     // 그리드 생성
//     grid = gtk_grid_new();
//     gtk_container_add(GTK_CONTAINER(window), grid);

//     // 로그인 버튼 생성 및 설정
//     login_button = gtk_button_new_with_label("Login");
//     g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), NULL);
//     gtk_grid_attach(GTK_GRID(grid), login_button, 1, 1, 1, 1);

//     // 윈도우 크기 설정
//     gtk_widget_set_size_request(window, 300, 200);

//     // 윈도우를 보이게 함
//     gtk_widget_show_all(window);

//     // GTK 메인 루프 실행
//     gtk_main();

//     return 0;
// }
