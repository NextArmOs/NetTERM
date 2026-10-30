use eframe::egui;
use serde::{Deserialize, Serialize};
use std::path::PathBuf;
use std::fs;

#[derive(Clone, Serialize, Deserialize, PartialEq)]
pub enum AppType {
    Editor,
    Monitor,
    FileManager,
    FileTextEditor { file_path: PathBuf },
}

#[derive(Clone, Serialize, Deserialize)]
pub struct Instance {
    pub id: String,
    pub title: String,
    pub app_type: AppType,
    pub is_open: bool,
    pub is_maximized: bool,
}

#[derive(Clone, Copy, PartialEq, Serialize, Deserialize)]
pub enum SortOrder {
    ByName,
    ByDate,
}

pub fn show(ui: &mut egui::Ui) {
    let ctx = ui.ctx();
    let desktop_rect = ctx.available_rect();

    let list_key = egui::Id::new("wm_instances_list");
    let mut instances = ctx.data_mut(|d| d.get_temp::<Vec<Instance>>(list_key).unwrap_or_else(|| vec![
        Instance {
            id: "sys_monitor_0".to_string(),
            title: "System Monitor".to_string(),
            app_type: AppType::Monitor,
            is_open: true,
            is_maximized: false,
        }
    ]));

    let count_key = egui::Id::new("wm_instances_counter");
    let mut instance_counter = ctx.data_mut(|d| d.get_temp::<usize>(count_key).unwrap_or(1));
    egui::TopBottomPanel::top("nothing_ui_panel")
        .frame(egui::Frame::none()
            .fill(egui::Color32::from_rgb(28, 28, 28))
            .inner_margin(egui::Margin::symmetric(14.0, 8.0))
        )
        .show(ctx, |panel_ui| {
            panel_ui.horizontal(|h_ui| {
                h_ui.colored_label(egui::Color32::from_rgb(240, 240, 240), "nothingUI");
                h_ui.separator();

                if h_ui.button("New Editor").clicked() {
                    instances.push(Instance {
                        id: format!("editor_{}", instance_counter),
                        title: format!("Editor #{}", instance_counter + 1),
                        app_type: AppType::Editor,
                        is_open: true,
                        is_maximized: false,
                    });
                    instance_counter += 1;
                }

                if h_ui.button("Files").clicked() {
                    instances.push(Instance {
                        id: format!("filemanager_{}", instance_counter),
                        title: format!("File Manager"),
                        app_type: AppType::FileManager,
                        is_open: true,
                        is_maximized: false,
                    });
                    instance_counter += 1;
                }

                h_ui.with_layout(egui::Layout::right_to_left(egui::Align::Center), |r_ui| {
                    let version_string = format!("NetTERM v{}", env!("CARGO_PKG_VERSION"));
                    r_ui.label(egui::RichText::new(version_string).color(egui::Color32::from_gray(140)));
                    r_ui.separator();
                    r_ui.label(egui::RichText::new("Mon, 26/10 • 02:26").color(egui::Color32::from_gray(180)));
                });
            });
        });
    egui::TopBottomPanel::bottom("nothing_ui_taskbar")
        .frame(egui::Frame::none()
            .fill(egui::Color32::from_rgb(22, 22, 22))
            .inner_margin(egui::Margin::symmetric(15.0, 8.0))
        )
        .show(ctx, |panel_ui| {
            panel_ui.horizontal(|h_ui| {
                for instance in &mut instances {
                    let indicator = if instance.is_open { "●" } else { "○" };
                    if h_ui.selectable_label(instance.is_open, format!("{} {}", indicator, instance.title)).clicked() {
                        instance.is_open = !instance.is_open;
                    }
                    h_ui.add_space(4.0);
                }
            });
        });
    egui::CentralPanel::default()
        .frame(egui::Frame::none().fill(egui::Color32::from_rgb(18, 18, 18)))
        .show(ctx, |_| {});

    let mut window_style = (*ctx.style()).clone();
    window_style.visuals.window_rounding = egui::Rounding::same(16.0); 
    window_style.visuals.window_fill = egui::Color32::from_rgb(30, 30, 34); 
    window_style.visuals.window_stroke = egui::Stroke::new(1.0, egui::Color32::from_gray(40));
    window_style.visuals.widgets.noninteractive.bg_fill = egui::Color32::from_rgb(30, 30, 34);
    
    window_style.visuals.widgets.inactive.rounding = egui::Rounding::same(6.0);
    window_style.visuals.widgets.hovered.rounding = egui::Rounding::same(6.0);
    window_style.visuals.widgets.active.rounding = egui::Rounding::same(6.0);
    ctx.set_style(window_style);

    let mut new_instances_to_spawn = vec![];

    for instance in &mut instances {
        if !instance.is_open {
            continue;
        }

        let mut is_current_open = true;
        let window_id = egui::Id::new(&instance.id);
        
        let mut window = egui::Window::new(&instance.title)
            .id(window_id)
            .open(&mut is_current_open)
            .collapsible(false);

        if instance.is_maximized {
            window = window
                .fixed_rect(desktop_rect)
                .movable(false)
                .resizable(false);
        } else {
            window = window
                .resizable(true)
                .movable(true)
                .default_size([900.0, 560.0]);
        }

        window.show(ctx, |win_ui| {
            win_ui.horizontal(|h_ui| {
                let btn_text = if instance.is_maximized { "Minimize" } else { "Maximize" };
                if h_ui.button(btn_text).clicked() {
                    instance.is_maximized = !instance.is_maximized;
                }
                h_ui.separator();
            });
            win_ui.add_space(6.0);

            match &instance.app_type {
                AppType::Monitor => {
                    win_ui.label("System status: operational");
                }
                AppType::Editor => {
                    let text_state_key = egui::Id::new(format!("txt_state_{}", instance.id));
                    let mut buffer = ctx.data_mut(|d| d.get_temp::<String>(text_state_key).unwrap_or_default());
                    
                    let edit_box = egui::TextEdit::multiline(&mut buffer)
                        .desired_width(f32::INFINITY)
                        .desired_rows(1);
                        
                    let scroll_id = egui::Id::new(format!("scroll_ed_{}", instance.id));
                    let mut changed = false;
                    
                    egui::ScrollArea::vertical()
                        .id_source(scroll_id)
                        .show(win_ui, |scroll_ui| {
                            if scroll_ui.add_sized(scroll_ui.available_size(), edit_box).changed() {
                                changed = true;
                            }
                        });

                    if changed {
                        ctx.data_mut(|d| d.insert_temp(text_state_key, buffer));
                    }
                }
                AppType::FileTextEditor { file_path } => {
                    let text_state_key = egui::Id::new(format!("file_edit_state_{}", instance.id));
                    let mut buffer = ctx.data_mut(|d| {
                        d.get_temp::<String>(text_state_key).unwrap_or_else(|| {
                            fs::read_to_string(file_path).unwrap_or_else(|_| "".to_string())
                        })
                    });

                    win_ui.horizontal(|h_ui| {
                        if h_ui.button("Save").clicked() {
                            let _ = fs::write(file_path, &buffer);
                        }
                        h_ui.label(egui::RichText::new(format!("{}", file_path.display())).small().color(egui::Color32::GRAY));
                    });
                    win_ui.add_space(6.0);

                    let edit_box = egui::TextEdit::multiline(&mut buffer)
                        .desired_width(f32::INFINITY)
                        .desired_rows(1);

                    let scroll_id = egui::Id::new(format!("scroll_file_ed_{}", instance.id));
                    let mut changed = false;

                    egui::ScrollArea::vertical()
                        .id_source(scroll_id)
                        .show(win_ui, |scroll_ui| {
                            if scroll_ui.add_sized(scroll_ui.available_size(), edit_box).changed() {
                                changed = true;
                            }
                        });

                    if changed {
                        ctx.data_mut(|d| d.insert_temp(text_state_key, buffer));
                    }
                }
                AppType::FileManager => {
                    let path_key = egui::Id::new(format!("fm_path_{}", instance.id));
                    let sort_key = egui::Id::new(format!("fm_sort_{}", instance.id));
                    let new_folder_key = egui::Id::new(format!("fm_new_folder_{}", instance.id));
                    
                    let mut current_path = ctx.data_mut(|d| {
                        d.get_temp::<PathBuf>(path_key).unwrap_or_else(|| {
                            std::env::current_dir().unwrap_or_else(|_| PathBuf::from("."))
                        })
                    });

                    let mut current_sort = ctx.data_mut(|d| {
                        d.get_temp::<SortOrder>(sort_key).unwrap_or(SortOrder::ByName)
                    });

                    let mut new_folder_name = ctx.data_mut(|d| {
                        d.get_temp::<String>(new_folder_key).unwrap_or_default()
                    });

                    let home_dir = std::env::var_os("HOME")
                        .map(PathBuf::from)
                        .unwrap_or_else(|| std::env::current_dir().unwrap_or_else(|_| PathBuf::from(".")));

                    win_ui.horizontal(|cols_ui| {
      cols_ui.vertical(|side_ui| {
      side_ui.set_width(160.0);
      side_ui.label(egui::RichText::new("System").strong().color(egui::Color32::from_gray(180)));
      side_ui.add_space(8.0);
if side_ui.selectable_label(current_path == home_dir, "Home").clicked() {
current_path = home_dir.clone();
}
if side_ui.selectable_label(current_path == home_dir.join("Documents"), "Documents").clicked() {
current_path = home_dir.join("Documents");
}
if side_ui.selectable_label(current_path == home_dir.join("Downloads"), "Downloads").clicked() {
current_path = home_dir.join("Downloads");
}
if side_ui.selectable_label(current_path == home_dir.join("Pictures"), "Pictures").clicked() {
current_path = home_dir.join("Pictures");
}
if side_ui.selectable_label(current_path == home_dir.join("Videos"), "Videos").clicked() {
current_path = home_dir.join("Videos");
}
});
cols_ui.separator();
let remaining_w = cols_ui.available_width();
let remaining_h = cols_ui.available_height();
cols_ui.allocate_ui_with_layout(
egui::vec2(remaining_w, remaining_h),
egui::Layout::top_down(egui::Align::Min),
|main_ui| {
main_ui.horizontal(|nav_ui| {
if nav_ui.button("Back").clicked() {
if let Some(parent) = current_path.parent() {
current_path = parent.to_path_buf();
}
}
nav_ui.separator();
nav_ui.label(egui::RichText::new(format!("{}", current_path.display())).monospace().color(egui::Color32::from_gray(200)));
nav_ui.with_layout(egui::Layout::right_to_left(egui::Align::Center), |combo_ui| {
egui::ComboBox::from_id_source(format!("sort_combo_{}", instance.id))
.selected_text(match current_sort {
SortOrder::ByName => "Sort: Name",
SortOrder::ByDate => "Sort: Date",
})
.show_ui(combo_ui, |menu_ui| {
menu_ui.selectable_value(&mut current_sort, SortOrder::ByName, "Name");
menu_ui.selectable_value(&mut current_sort, SortOrder::ByDate, "Date");
});
});
});
main_ui.add_space(10.0);
main_ui.horizontal(|folder_ui| {
folder_ui.add(egui::TextEdit::singleline(&mut new_folder_name).hint_text("New folder name..."));
if folder_ui.button("Create Folder").clicked() && !new_folder_name.trim().is_empty() {
let target = current_path.join(new_folder_name.trim());
let _ = fs::create_dir_all(target);
new_folder_name.clear();
}
});
main_ui.add_space(14.0);
if let Ok(entries) = std::fs::read_dir(&current_path) {
let mut items: Vec<_> = entries.flatten().collect();
match current_sort {
SortOrder::ByName => {
items.sort_by(|a, b| a.file_name().cmp(&b.file_name()));
}
SortOrder::ByDate => {
items.sort_by(|a, b| {
let time_a = a.metadata().and_then(|m| m.modified()).unwrap_or(std::time::SystemTime::UNIX_EPOCH);
let time_b = b.metadata().and_then(|m| m.modified()).unwrap_or(std::time::SystemTime::UNIX_EPOCH);
time_b.cmp(&time_a)
});
}
}
let mut dirs = vec![];
let mut files = vec![];
for entry in items {
let path = entry.path();
if path.is_dir() { dirs.push(path); } else { files.push(path); }
}
let scroll_id = egui::Id::new(format!("scroll_fm_{}", instance.id));
let mut clicked_dir = None;
let mut delete_dir = None;
let mut open_file = None;
let mut delete_file = None;
egui::ScrollArea::vertical()
.id_source(scroll_id)
.show(main_ui, |scroll_ui| {
scroll_ui.horizontal_wrapped(|wrap_ui| {
wrap_ui.spacing_mut().item_spacing = egui::vec2(14.0, 14.0);
for dir in &dirs {
let dir_name = dir.file_name().unwrap_or_default().to_string_lossy().into_owned();
wrap_ui.vertical(|item_ui| {
item_ui.set_max_width(95.0);
if item_ui.add(egui::Button::new(format!("Folder:\n{}", dir_name)).min_size(egui::vec2(95.0, 85.0)).fill(egui::Color32::from_rgb(44, 44, 48))).clicked() {
clicked_dir = Some(dir.clone());
}
if item_ui.small_button("Delete").clicked() {
delete_dir = Some(dir.clone());
}
});
}
for file in &files {
let file_name = file.file_name().unwrap_or_default().to_string_lossy().into_owned();
wrap_ui.vertical(|item_ui| {
item_ui.set_max_width(95.0);
if item_ui.add(egui::Button::new(format!("File:\n{}", file_name)).min_size(egui::vec2(95.0, 85.0)).fill(egui::Color32::from_rgb(44, 44, 48))).clicked() {
open_file = Some((file.clone(), file_name.clone()));
}
if item_ui.small_button("Delete").clicked() {
delete_file = Some(file.clone());
}
});
}
});
});
if let Some(path) = clicked_dir { current_path = path; }
if let Some(path) = delete_dir { let _ = fs::remove_dir_all(path); }
if let Some(path) = delete_file { let _ = fs::remove_file(path); }
if let Some((path, name)) = open_file {
new_instances_to_spawn.push(Instance {
id: format!("file_editor_{}", instance_counter),
title: format!("Edit: {}", name),
app_type: AppType::FileTextEditor { file_path: path },
is_open: true,
is_maximized: false,
});
instance_counter += 1;
}
}
}
);
});
ctx.data_mut(|d| {
d.insert_temp(path_key, current_path);
d.insert_temp(sort_key, current_sort);
d.insert_temp(new_folder_key, new_folder_name);
});
}
}
});
if !is_current_open {
instance.is_open = false;
}
}
if !new_instances_to_spawn.is_empty() {
instances.extend(new_instances_to_spawn);
}
ctx.data_mut(|d| {
d.insert_temp(list_key, instances);
d.insert_temp(count_key, instance_counter);
});
}
