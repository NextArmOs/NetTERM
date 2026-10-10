use eframe::egui;
use crate::PaintLine;

pub fn show(ui: &mut egui::Ui, paint_lines: &mut Vec<PaintLine>, current_brush_color: &mut egui::Color32) {
    ui.horizontal(|ui| {
        ui.heading("Canvas Paint Area");
        ui.separator();
        if ui.button("Clear Canvas").clicked() {
            paint_lines.clear();
        }
        ui.separator();
        ui.label("Brush Color:");
        ui.selectable_value(current_brush_color, egui::Color32::from_hex("#82a1c1").unwrap(), "Default");
        ui.selectable_value(current_brush_color, egui::Color32::from_hex("#00ff00").unwrap(), "Green");
        ui.selectable_value(current_brush_color, egui::Color32::from_hex("#00ffff").unwrap(), "Blue");
        ui.selectable_value(current_brush_color, egui::Color32::from_hex("#ff966c").unwrap(), "Orange");
        ui.selectable_value(current_brush_color, egui::Color32::from_hex("#ff757f").unwrap(), "Pink");
    });
    ui.add_space(5.0);
    ui.separator();

    let (response, painter) = ui.allocate_painter(ui.available_size(), egui::Sense::drag());
    
    if let Some(pointer_pos) = response.interact_pointer_pos() {
        if response.dragged_by(egui::PointerButton::Primary) {
            if response.drag_started_by(egui::PointerButton::Primary) {
                paint_lines.push(PaintLine {
                    points: vec![pointer_pos],
                    color: *current_brush_color,
                });
            } else if let Some(last_line) = paint_lines.last_mut() {
                if last_line.points.last() != Some(&pointer_pos) {
                    last_line.points.push(pointer_pos);
                }
            }
        }
    }

    for line in paint_lines {
        if line.points.len() >= 2 {
            painter.add(egui::Shape::line(line.points.clone(), egui::Stroke::new(2.5, line.color)));
        }
    }
}
