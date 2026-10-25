use eframe::egui;
use serde::{Deserialize, Serialize};

#[derive(Clone, Serialize, Deserialize, PartialEq)]
pub enum AppType {
    Editor,
    Monitor,
}

#[derive(Clone, Serialize, Deserialize)]
pub struct Instance {
    pub id: String,
    pub title: String,
    pub app_type: AppType,
    pub is_open: bool,
}

pub fn show(ui: &mut egui::Ui) {
    let ctx = ui.ctx();
    let desktop_rect = ctx.available_rect();
    let painter = ui.painter();
    
    painter.rect_filled(
        desktop_rect,
        0.0,
        egui::Color32::from_rgb(15, 16, 32),
    );

    let list_key = egui::Id::new("wm_instances_list");
    let mut instances = ctx.data_mut(|d| d.get_temp::<Vec<Instance>>(list_key).unwrap_or_else(|| vec![
        Instance {
            id: "sys_monitor_0".to_string(),
            title: "Monitor #1".to_string(),
            app_type: AppType::Monitor,
            is_open: true,
        }
    ]));

    let count_key = egui::Id::new("wm_instances_counter");
    let mut instance_counter = ctx.data_mut(|d| d.get_temp::<usize>(count_key).unwrap_or(1));

    egui::TopBottomPanel::top("nothing_ui_panel")
        .frame(egui::Frame::none()
            .fill(egui::Color32::from_rgba_unmultiplied(25, 27, 50, 230))
            .inner_margin(egui::Margin::symmetric(12.0, 10.0))
        )
        .show(ctx, |ui| {
            ui.horizontal(|ui| {
                ui.colored_label(egui::Color32::from_rgb(0, 255, 200), "nothingUI v0.0.2");
                ui.separator();

                if ui.button("➕ New Editor").clicked() {
                    instances.push(Instance {
                        id: format!("editor_{}", instance_counter),
                        title: format!("Editor #{}", instance_counter + 1),
                        app_type: AppType::Editor,
                        is_open: true,
                    });
                    instance_counter += 1;
                }

                if ui.button("New Monitor").clicked() {
                    instances.push(Instance {
                        id: format!("monitor_{}", instance_counter),
                        title: format!("Monitor #{}", instance_counter + 1),
                        app_type: AppType::Monitor,
                        is_open: true,
                    });
                    instance_counter += 1;
                }

                ui.with_layout(egui::Layout::right_to_left(egui::Align::Center), |ui| {
                    ui.label("19:18");
                });
            });
        });

    egui::TopBottomPanel::bottom("nothing_ui_taskbar")
        .frame(egui::Frame::none()
            .fill(egui::Color32::from_rgba_unmultiplied(20, 22, 40, 240))
            .inner_margin(egui::Margin::symmetric(15.0, 8.0))
        );
            egui::TopBottomPanel::bottom("nothing_ui_taskbar")
        .frame(egui::Frame::none()
            .fill(egui::Color32::from_rgba_unmultiplied(20, 22, 40, 240))
            .inner_margin(egui::Margin::symmetric(15.0, 8.0))
        )
        .show(ctx, |ui| {
            ui.horizontal(|ui| {
                for instance in &mut instances {
                    let indicator = if instance.is_open { "●" } else { "○" };
                    if ui.selectable_label(instance.is_open, format!("{} {}", indicator, instance.title)).clicked() {
                        instance.is_open = !instance.is_open;
                    }
                    ui.add_space(4.0);
                }
            });
        });

    let mut window_style = (*ctx.style()).clone();
    window_style.visuals.window_rounding = egui::Rounding::same(4.0);
    ctx.set_style(window_style);

    for instance in &mut instances {
        if !instance.is_open {
            continue;
        }

        let mut is_current_open = true;

        match instance.app_type {
            AppType::Monitor => {
                egui::Window::new(&instance.title)
                    .id(egui::Id::new(&instance.id))
                    .open(&mut is_current_open)
                    .default_size([350.0, 220.0])
                    .resizable([true, true])
                    .vscroll(true)
                    .hscroll(true)
                    .show(ctx, |ui| {
                        ui.label("Status: Active");
                        ui.label(format!("Process ID: {}", instance.id));
                        ui.allocate_space(ui.available_size());
                    });
            }
            AppType::Editor => {
                egui::Window::new(&instance.title)
                    .id(egui::Id::new(&instance.id))
                    .open(&mut is_current_open)
                    .default_size([400.0, 300.0])
                    .resizable([true, true])
                    .vscroll(true)
                    .hscroll(true)
                    .show(ctx, |ui| {
                        let text_state_key = egui::Id::new(format!("txt_state_{}", instance.id));
                        let mut buffer = ctx.data_mut(|d| d.get_temp::<String>(text_state_key).unwrap_or_default());

                        if egui::TextEdit::multiline(&mut buffer)
                            .desired_rows(10)
                            .desired_width(f32::INFINITY)
                            .show(ui)
                            .response
                            .changed() 
                        {
                            ctx.data_mut(|d| d.insert_temp(text_state_key, buffer.clone()));
                        }

                        ui.allocate_space(ui.available_size());
                    });
            }
        }

        if !is_current_open {
            instance.is_open = false;
        }
    }

    ctx.data_mut(|d| {
        d.insert_temp(list_key, instances);
        d.insert_temp(count_key, instance_counter);
    });
}
