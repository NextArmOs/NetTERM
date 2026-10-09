use eframe::egui;

fn main() -> eframe::Result<()> {
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default().with_inner_size([400.0, 300.0]),
        ..Default::default()
    };
    
    let mut counter = 0;

    eframe::run_simple_native("NetTERM v0.1", options, move |ctx, _frame| {
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.heading("Welcome to NetTERM!");
            ui.separator();

            ui.label(format!("You have clicked the button {} time(s).", counter));

            if ui.button("Click to increase counter").clicked() {
                counter += 1;
            }

            ui.separator();

            if ui.button("Exit Application").clicked() {
                ctx.send_viewport_cmd(egui::ViewportCommand::Close);
            }
        });
    })
}

