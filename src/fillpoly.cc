#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <algorithm>
#include "scene/scene.hh"
#include "geometry.hh"
#include "menu/menu.hh"
#include "collections/collections.hh"

// Vars
const int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
int c_screen_width = SCREEN_WIDTH, c_screen_height = SCREEN_HEIGHT;
const float VW = (float)SCREEN_WIDTH / 100, VH = (float)SCREEN_HEIGHT / 100;
float cvw = VW, cvh = VH;
const SDL_Color SDL_COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color SDL_COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color SDL_COLOR_RED = {255, 0, 0, 255};
const SDL_Color SDL_COLOR_TRANSPARENT = {0, 0, 0, 0};

// Assets
Scene *main_scene;
SDL_Window *main_window;
TTF_Font *default_font;
Button *save_btn, *load_btn, *reset_btn, *background_btn;
Label *message_text, *thickness_label, *edit_message, *delete_message;
CEditor ceditor;
VEditor veditor;
PolygonList *polygon_list;
DialogBox *thickness_control;
ColorWheel *color_wheel, *bg_color_wheel;
LightBar *light_bar, *bg_light_bar;
Canvas *canvas;

void load_assets()
{
    printf("Loading Assets...");
    default_font = TTF_OpenFont("/home/pedrozoz/Repositories/University/cg/fillpoly/assets/fonts/open-sans/OpenSans-Regular.ttf", 14);
    if (default_font == NULL)
        std::exit(1);
    printf("Assets loaded!\n");
}

void resize(int width, int height)
{
    // canvas->clear();
    c_screen_width = width;
    c_screen_height = height;
    cvw = (float)c_screen_width / 100;
    cvh = (float)c_screen_height / 100;
    canvas->set_geometry(30 * cvw, 5 * cvh, 65 * cvw, 90 * cvh);
    edit_message->set_position(canvas->geometry.x + (canvas->geometry.w / 2) - edit_message->geometry.w / 2, 2.5 * cvh - edit_message->geometry.h / 2);
    message_text->set_position(canvas->geometry.x + (canvas->geometry.w / 2) - message_text->geometry.w / 2, 95 * cvh);
    // Colors
    float dialogs_area = 18 * cvw;
    float dialogs_gap = 1 * cvw;
    float dialogs_y = 90 * cvh;
    ceditor.update_geometry((30 * cvw) / 2 - ceditor.geometry.w / 2, 90 * cvh - 10 * cvh, 18 * cvw, 10 * cvh);
    color_wheel->set_position(ceditor.geometry.x + (ceditor.geometry.w / 2) - color_wheel->geometry.w / 2, ceditor.geometry.y - color_wheel->geometry.h);
    color_wheel->set_radius(5 * cvw);
    light_bar->set_dimensions(2 * cvw, color_wheel->geometry.h);
    light_bar->set_position(color_wheel->geometry.x + color_wheel->geometry.w + 1 * cvw, color_wheel->geometry.y);
    // Menu
    float menu_gap = 1 * cvh;
    reset_btn->set_geometry((30 * cvw) / 2 - (10 * cvw) / 2, color_wheel->geometry.y - reset_btn->geometry.h - menu_gap, 10 * cvw, 5 * cvh);
    thickness_control->set_dimensions(10 * cvw, 5 * cvh);
    thickness_control->set_position(reset_btn->geometry.x + (reset_btn->geometry.w / 2) - thickness_control->geometry.w / 2, reset_btn->geometry.y - thickness_control->geometry.h - menu_gap);
    thickness_label->set_position(thickness_control->geometry.x + thickness_control->geometry.w, thickness_control->geometry.y + (thickness_control->geometry.h / 2) - thickness_label->geometry.h / 2);
    background_btn->set_dimensions(15 * cvw, 5 * cvh);
    background_btn->set_position(reset_btn->geometry.x + (reset_btn->geometry.w / 2) - background_btn->geometry.w / 2, thickness_control->geometry.y - background_btn->geometry.h - menu_gap);
    bg_color_wheel->set_radius(5 * cvw);
    bg_color_wheel->set_position(background_btn->geometry.x + (background_btn->geometry.w / 2) - bg_color_wheel->geometry.w / 2, background_btn->geometry.y - bg_color_wheel->geometry.h - menu_gap);
    bg_light_bar->set_dimensions(2 * cvw, bg_color_wheel->geometry.h);
    bg_light_bar->set_position(bg_color_wheel->geometry.x + bg_color_wheel->geometry.w + 1 * cvw, bg_color_wheel->geometry.y);
    polygon_list->set_dimensions(10 * cvw);
    polygon_list->set_position((30 * cvw) / 2 - polygon_list->geometry.w / 2, 10 * cvh);
    delete_message->set_position(polygon_list->geometry.x + (polygon_list->geometry.w / 2) - delete_message->geometry.w / 2, polygon_list->geometry.y - polygon_list->geometry.h);
    if (veditor.is_selected)
    {
        int actual_x, actual_y;
        canvas->get_vertex_pos(veditor.vertex, &actual_x, &actual_y);
        veditor.update_geometry(actual_x - 9 * cvw, actual_y - 10 * cvh, 18 * cvw, 10 * cvh);
    }
    else
    {
        veditor.update_dimensions(18 * cvw, 10 * cvh);
    }
}

void lightbar_onclick(int x, int y, bool hit)
{
    if (hit)
    {
        light_bar->set_luminance(x, y);
        color_wheel->set_luminance((float)light_bar->get_luminance()/255);
    }
}

void bg_lightbar_onclick(int x, int y, bool hit)
{
    if (hit)
    {
        bg_light_bar->set_luminance(x, y);
        bg_color_wheel->set_luminance((float)bg_light_bar->get_luminance()/255);
    }
}

void canvas_onclick(int x, int y, bool hit)
{
    if (hit && !veditor.is_selected)
    {
        SDL_Color color = {std::stoi(ceditor.red_editor->get_text_content()), std::stoi(ceditor.green_editor->get_text_content()), std::stoi(ceditor.blue_editor->get_text_content()), 255};
        if (canvas->current_vertices == 0)
        {
            message_text->update_text("Clique com o botão direito para criar o polígono. (É necessário 3+ vértices)");
            message_text->set_position(canvas->geometry.x + (canvas->geometry.w / 2) - message_text->geometry.w / 2, 95 * cvh);
            canvas->open_polygon(x, y, color);
        }
        else
            canvas->draw_polygon(x, y, color);
    }
    else if (hit && veditor.is_selected)
        veditor.unselect();
}

void canvas_onright_click(int x, int y, bool hit)
{
    if (canvas->current_vertices > 2 && hit)
    {
        message_text->update_text("Clique com o botão esquerdo para começar a desenhar um polígono.");
        message_text->set_position(canvas->geometry.x + (canvas->geometry.w / 2) - message_text->geometry.w / 2, 95 * cvh);
        polygon_list->add_polygon(canvas->close_polygon());
    }
}

void canvas_onmiddle_click(int x, int y, bool hit)
{
    if (x > canvas->geometry.x && x < canvas->geometry.x + canvas->geometry.w && y > canvas->geometry.y && y < canvas->geometry.y + canvas->geometry.h)
    {
        Vertex *selected = canvas->first_vertex_touched(x, y, (c_screen_width + c_screen_height) * 0.012);
        if (selected != NULL && selected != veditor.vertex)
        {
            int screen_x, screen_y;
            canvas->get_vertex_pos(selected, &screen_x, &screen_y);
            veditor.select_vertex(selected);
            veditor.update_position(screen_x - veditor.geometry.w / 2, screen_y - selected->thickness - veditor.geometry.h);
        }
        else
            veditor.unselect();
    }
}

void handle_miss_click(int x, int y, bool hit)
{
    if (!hit && veditor.is_selected)
        veditor.unselect();
}

void handle_colorwheel_click(int x, int y, bool hit)
{
    if (hit)
    {
        int red = -1, green = -1, blue = -1;
        color_wheel->get_color(x, y, &red, &green, &blue);
        if (red != -1)
        {
            ceditor.red_editor->update_text(std::to_string(red));
            ceditor.green_editor->update_text(std::to_string(green));
            ceditor.blue_editor->update_text(std::to_string(blue));
        }
    }
}

void handle_reset_onlick(int x, int y, bool hit)
{
    if (hit)
    {
        canvas->clear();
        polygon_list->clear();
        message_text->update_text("Clique com o botão esquerdo para começar a desenhar um polígono.");
        if (veditor.is_selected)
            veditor.unselect();
    }
}

bool reset_btn_on = false;

void handle_veditor_redinput(SDL_TextInputEvent event, Component *target)
{
    DialogBox *dialog = (DialogBox *)target;
    int n = std::stoi(dialog->get_text_content());
    if (n > 255)
    {
        dialog->update_text("255");
    }
    else if (n < 0)
    {
        dialog->update_text("0");
    }
    veditor.update_red();
    canvas->set_colors();
}

void handle_veditor_greeninput(SDL_TextInputEvent event, Component *target)
{
    DialogBox *dialog = (DialogBox *)target;
    int n = std::stoi(dialog->get_text_content());
    if (n > 255)
    {
        dialog->update_text("255");
    }
    else if (n < 0)
    {
        dialog->update_text("0");
    }
    veditor.update_green();
    canvas->set_colors();
}

void handle_veditor_blueinput(SDL_TextInputEvent event, Component *target)
{
    DialogBox *dialog = (DialogBox *)target;
    int n = std::stoi(dialog->get_text_content());
    if (n > 255)
    {
        dialog->update_text("255");
    }
    else if (n < 0)
    {
        dialog->update_text("0");
    }
    veditor.update_blue();
    canvas->set_colors();
}

void reset_btn_animation(int x, int y, bool hit)
{
    if (hit && !reset_btn_on)
    {
        reset_btn->set_border_color({0, 0, 0, 100});
        reset_btn_on = true;
    }
    else if (!hit && reset_btn_on)
    {
        reset_btn->set_border_color({0, 0, 0, 255});
        reset_btn_on = false;
    }
}

bool background_btn_on = false;

void background_btn_animation(int x, int y, bool hit)
{
    if (hit && !background_btn_on)
    {
        background_btn->set_border_color({0, 0, 0, 100});
        background_btn_on = true;
    }
    else if (!hit && background_btn_on)
    {
        background_btn->set_border_color({0, 0, 0, 255});
        background_btn_on = false;
    }
}

void thickness_control_animation(int x, int y, bool hit)
{
    if (hit && !thickness_label->is_visible())
        thickness_label->set_visible(true);
    else if (!hit && thickness_label->is_visible())
        thickness_label->set_visible(false);
}

void handle_background_btn_onclick(int x, int y, bool hit)
{
    if (hit)
    {
        if (!bg_color_wheel->is_visible())
        {
            bg_color_wheel->set_visible(true);
            bg_light_bar->set_visible(true);
        }
        else
        {
            bg_color_wheel->set_visible(false);
            bg_light_bar->set_visible(false);
        }
    }
}

void handle_thicknessinput(SDL_TextInputEvent event, Component *target)
{
    DialogBox *dialog = (DialogBox *)target;
    int n = std::stoi(dialog->get_text_content());
    if (n > 10)
    {
        n = 10;
        dialog->update_text("10");
    }
    else if (n < 1)
    {
        n = 1;
        dialog->update_text("1");
    }
    canvas->set_thickness(n);
}

void handle_on_bg_color_click(int x, int y, bool hit)
{
    int red = -1, blue = -1, green = -1;
    if (hit)
    {
        bg_color_wheel->get_color(x, y, &red, &blue, &green);
        if (red != -1 && blue != -1 && green != -1)
        {
            SDL_Color inverse = {255 - (Uint8)red, 255 - (Uint8)blue, 255 - (Uint8)green, 255};
            veditor.red->set_text_color(inverse);
            veditor.green->set_text_color(inverse);
            veditor.blue->set_text_color(inverse);
            veditor.red->set_border_color(inverse);
            veditor.green->set_border_color(inverse);
            veditor.blue->set_border_color(inverse);
            canvas->set_fill({(Uint8)red, (Uint8)blue, (Uint8)green, 255});
        }
    }
}

void polygon_list_onclick(int x, int y, bool hit)
{
    if (hit)
    {
        Polygon *polygon = polygon_list->remove_polygon(x, y);
        if (polygon != NULL)
        {
            canvas->delete_polygon(polygon);
            if (veditor.is_selected)
                veditor.unselect();
        }
    }
}

int main(int argc, char *args[])
{
    /* Resources Init */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Não pode inicializar as funções gráficas do SDL. Encerrando...\n");
        std::exit(1);
    }
    if (TTF_Init() < 0)
    {
        printf("Não pode inicializar as fontes do SDL. Encerrando...\n");
        std::exit(1);
    }
    /* Create Scene */
    printf("Creating scene... ");
    main_window = SDL_CreateWindow("Fillpoly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    main_scene = new Scene(main_window);
    main_scene->fill(SDL_COLOR_WHITE);
    printf("Scene Created!\n");
    /* Load Assets */
    load_assets();
    /* Create Components */
    edit_message = new Label("Clique com o botão do meio do mouse em um vértice para edita-lo.", default_font, 19);
    thickness_label = new Label("Tamanho do Vértice", default_font, 19);
    delete_message = new Label("Clique em um polígono para deletá-lo.", default_font, 19);
    thickness_control = new DialogBox("1", 22, default_font, 10 * VW, 4 * VH);
    canvas = new Canvas(65 * VW, 90 * VH);
    canvas->set_thickness(1);
    message_text = new Label("Clique com o botão esquerdo para começar a desenhar um polígono.", default_font, 22);
    ceditor = CEditor((30 * VW) / 2 - ceditor.geometry.w / 2, 90 * cvw - 10 * VH, 18 * VW, 10 * VH, default_font);
    veditor = VEditor((30 * VW) / 2 - veditor.geometry.w / 2, 90 * cvw - 10 * VH, 18 * VW, 10 * VH, default_font);
    color_wheel = new ColorWheel(ceditor.geometry.x + (ceditor.geometry.w / 2) - (5 * VW) / 2, ceditor.geometry.y - 10 * VW, 5 * VW);
    bg_color_wheel = new ColorWheel(color_wheel->geometry.w, ceditor.geometry.h, 5 * VW);
    bg_color_wheel->set_visible(false);
    light_bar = new LightBar(2 * VW, color_wheel->geometry.h);
    bg_light_bar = new LightBar(2 * VW, color_wheel->geometry.h);
    bg_light_bar->set_visible(false);
    reset_btn = new Button("Limpar", 10 * VW, 5 * VH, default_font);
    background_btn = new Button("Mudar Cor de Fundo", 20 * VW, 5 * VH, default_font);
    polygon_list = new PolygonList(10 * cvw);
    resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    /* Components Setup */
    // Vertex Editor
    canvas->z_index = 0;
    veditor.red->set_ontextinput(handle_veditor_redinput);
    veditor.green->set_ontextinput(handle_veditor_greeninput);
    veditor.blue->set_ontextinput(handle_veditor_blueinput);
    veditor.red->z_index = 1;
    veditor.green->z_index = 1;
    veditor.blue->z_index = 1;
    // Canvas
    canvas->set_onclick(canvas_onclick);
    canvas->set_onright_click(canvas_onright_click);
    // Menu
    canvas->set_onmiddle_click(canvas_onmiddle_click);
    color_wheel->set_onclick(handle_colorwheel_click);
    reset_btn->set_onclick(handle_reset_onlick);
    reset_btn->set_onmotion(reset_btn_animation);
    thickness_label->set_visible(false);
    thickness_control->digits_only = true;
    thickness_control->set_default_text("1");
    thickness_control->set_ontextinput(handle_thicknessinput);
    thickness_control->set_onmotion(thickness_control_animation);
    background_btn->set_onclick(handle_background_btn_onclick);
    background_btn->set_onmotion(background_btn_animation);
    bg_color_wheel->set_onclick(handle_on_bg_color_click);
    light_bar->set_onclick(lightbar_onclick);
    bg_light_bar->set_onclick(bg_lightbar_onclick);
    polygon_list->set_onclick(polygon_list_onclick);
    /* Add Components */
    main_scene->add_component(canvas);
    main_scene->add_component(message_text);
    main_scene->add_component(edit_message);
    main_scene->add_collection(&ceditor);
    main_scene->add_collection(&veditor);
    main_scene->add_component(color_wheel);
    main_scene->add_component(reset_btn);
    main_scene->add_component(thickness_control);
    main_scene->add_component(thickness_label);
    main_scene->add_component(background_btn);
    main_scene->add_component(bg_color_wheel);
    main_scene->add_component(light_bar);
    main_scene->add_component(bg_light_bar);
    main_scene->add_component(polygon_list);
    main_scene->add_component(delete_message);
    main_scene->onresize(resize);
    /* Main Loop */
    while (!main_scene->quit)
    {
        main_scene->poll();
        main_scene->render();
    }
    main_scene->destroy();
    delete main_scene;
    return 0;
}