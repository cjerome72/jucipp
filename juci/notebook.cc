#include "notebook.h"


Notebook::View::View() :
  view_(Gtk::ORIENTATION_VERTICAL){

}
Gtk::Box& Notebook::View::view() {
  view_.pack_start(notebook_);
  return view_;
}
Notebook::Controller::Controller(Keybindings::Controller& keybindings){

  scrolledwindow_vec_.push_back(new Gtk::ScrolledWindow());
  source_vec_.push_back(new Source::Controller);
  scrolledwindow_vec_.back()->add(source_vec_.back()->view());
  source_vec_.back()->OnNewEmptyFile();
  view_.notebook().append_page(*scrolledwindow_vec_.back(), "juCi++");
  refClipboard = Gtk::Clipboard::get();

  keybindings.action_group_menu()->add(Gtk::Action::create("FileMenu",
                                                           Gtk::Stock::FILE));
  /* File->New files */

  keybindings.action_group_menu()->add(Gtk::Action::create("FileNewStandard",
                                                           Gtk::Stock::NEW,
                                                           "New empty file",
                                                           "Create a new file"),
                                       [this]() {
					 isnewfile = true;
                                         OnFileNewEmptyfile();
                                       });
  keybindings.action_group_menu()->add(Gtk::Action::create("FileNewCC",
                                                           "New cc file"),
                                       Gtk::AccelKey("<control><alt>c"),
                                       [this]() {
					 isnewfile = true;
                                         OnFileNewCCFile();
                                       });
  keybindings.action_group_menu()->add(Gtk::Action::create("FileNewH",
                                                           "New h file"),
                                       Gtk::AccelKey("<control><alt>h"),
                                       [this]() {
					 isnewfile = true;
                                         OnFileNewHeaderFile();
                                       });
  keybindings.action_group_menu()->add(Gtk::Action::create("WindowCloseTab",
                                                           "Close tab"),
                                       Gtk::AccelKey("<control>w"),
                                       [this]() {
                                         OnCloseCurrentPage();
                                       });
  keybindings.action_group_menu()->add(Gtk::Action::create("EditFind",
                                                           Gtk::Stock::FIND),
                                       [this]() {
					 isnewfile = false;
					 OnEditSearch();
                                         //TODO(Oyvang, Zalox, Forgi)Create function OnEditFind();
                                       });
  keybindings.action_group_menu()->add(Gtk::Action::create("EditCopy",
                                                           Gtk::Stock::COPY),  
                                       [this]() {
                                         OnEditCopy();
                                       });
  keybindings.action_group_menu()->add(Gtk::Action::create("EditCut",
                                                           Gtk::Stock::CUT),
                                       [this]() {
                                         OnEditCut();
                                       });
  keybindings.action_group_menu()->add(Gtk::Action::create("EditPaste",
                                                           Gtk::Stock::PASTE),
                                       [this]() {
                                         OnEditPaste();
                                       });
  entry_.view_.entry().signal_activate().connect(
                                                 [this]() {
						   if(isnewfile){
						     OnNewPage(entry_.text());
						   }else{
						     Search();
						   }
                                                   entry_.OnHideEntries();
                                                 });
}//Constructor
Gtk::Box& Notebook::Controller::view() {
  return view_.view();
}
Gtk::Box& Notebook::Controller::entry_view(){
  return entry_.view();
}
void Notebook::Controller::OnNewPage(std::string name) {
  scrolledwindow_vec_.push_back(new Gtk::ScrolledWindow());
  source_vec_.push_back(new Source::Controller);
  scrolledwindow_vec_.back()->add(source_vec_.back()->view());
  source_vec_.back()->OnNewEmptyFile();
  view_.notebook().append_page(*scrolledwindow_vec_.back(), name);
  view_.notebook().show_all_children();
  view_.notebook().set_focus_child(*scrolledwindow_vec_.back());
  view_.notebook().set_current_page(view_.notebook().get_n_pages()-1);
}
void Notebook::Controller::OnCloseCurrentPage() {
  //TODO (oyvang, zalox, forgi) Save a temp file, in case you close one you dont want to close?
  int page = view_.notebook().get_current_page();
  view_.notebook().remove_page(page);
  delete source_vec_.at(page);
  delete scrolledwindow_vec_.at(page);
  source_vec_.erase(source_vec_.begin()+ page);
  scrolledwindow_vec_.erase(scrolledwindow_vec_.begin()+page);
}
void Notebook::Controller::OnFileNewEmptyfile() {
  entry_.OnShowSetFilenName("");
}
void Notebook::Controller::OnFileNewCCFile() {
  entry_.OnShowSetFilenName(".cc");
}
void Notebook::Controller::OnFileNewHeaderFile() {
  entry_.OnShowSetFilenName(".h");
}
void Notebook::Controller::OnEditCopy() {
  if (view_.notebook().get_n_pages() != 0) {
    int source_pos = view_.notebook().get_current_page();
    Glib::RefPtr<Gtk::TextBuffer> buffer = source_vec_.at(source_pos)
      ->view().get_buffer();
    buffer->copy_clipboard(refClipboard);
  }
}
void Notebook::Controller::OnEditPaste() {
  if (view_.notebook().get_n_pages() != 0) {
    int source_pos = view_.notebook().get_current_page();
    Glib::RefPtr<Gtk::TextBuffer> buffer = source_vec_.at(source_pos)
      ->view().get_buffer();
    buffer->paste_clipboard(refClipboard);
  }
}
void Notebook::Controller::OnEditCut() {
  if (view_.notebook().get_n_pages() != 0) {
    int source_pos = view_.notebook().get_current_page();
    Glib::RefPtr<Gtk::TextBuffer> buffer = source_vec_.at(source_pos)
      ->view().get_buffer();
    buffer->cut_clipboard(refClipboard);
  }
}

void Notebook::Controller::OnOpenFile(std::string path) {
  scrolledwindow_vec_.push_back(new Gtk::ScrolledWindow());
  source_vec_.push_back(new Source::Controller);
  scrolledwindow_vec_.back()->add(source_vec_.back()->view());
  source_vec_.back()->OnOpenFile(path);
  unsigned pos = path.find_last_of("/\\");
  view_.notebook().append_page(*scrolledwindow_vec_.back(), path.substr(pos+1));
  view_.notebook().show_all_children();
  view_.notebook().set_focus_child(*scrolledwindow_vec_.back());
  view_.notebook().set_current_page(view_.notebook().get_n_pages()-1);
}

std::string Notebook::Controller::GetCursorWord(){
  int page = view_.notebook().get_current_page();
  std::string word;
  Gtk::TextIter start,end;
  start=source_vec_.at(page)->view().get_buffer()->begin();
  end=source_vec_.at(page)->view().get_buffer()->end();
  // if(!source_vec_.at(page)->view().get_buffer()->get_selection_bounds(start,
  //								      end)){
    start=source_vec_.at(page)->view().get_buffer()->get_insert()->get_iter();
    end=source_vec_.at(page)->view().get_buffer()->get_insert()->get_iter();
    if(!end.ends_line()) {
      while(!end.ends_word()){
	end.forward_char();
      }
    }
    if(!start.starts_line()) {
      while(!start.starts_word()){
	start.backward_char();
      }
    }
    word = source_vec_.at(page)->view().get_buffer()->get_text(start,end);

    //TODO(Oyvang)fix selected text
   return word;
}

void Notebook::Controller::OnEditSearch(){
  entry_.OnShowSearch(GetCursorWord());   
   
}

void Notebook::Controller::Search(){
  std::string search_word;
  search_word = entry_.text();
  //TODO(oyvang) create seacrh function
  std::cout<<"funka"<<std::endl;
}


