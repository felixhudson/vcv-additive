#include "plugin.hpp"


struct Additive : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		PATH118_INPUT,
		PATH120_INPUT,
		CLKIN_INPUT,
		NEG1_INPUT,
		NEG5_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		PATH116_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Additive() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(PATH118_INPUT, "");
		configInput(PATH120_INPUT, "");
		configInput(CLKIN_INPUT, "");
		configOutput(PATH116_OUTPUT, "");
	}
  float pitch =1.f;
  float clock = 0.f;
  float top = 1.f;
  float bottom = -1.f;
  float value = 0.f;
  float debug = 0.f;
  // 
  bool onest = 1;
  bool fivest = 1;
  bool negonest = 1;
  bool negfivest = 1;
  int notecount = 0;
  int major[7] = {0,2,4,5,7,9,11};


	void process(const ProcessArgs& args) override {

    float clock_value = inputs[CLKIN_INPUT].getVoltage();
    value = inputs[PATH118_INPUT].getVoltage();
    /* if (value != debug){ */
      /* debug = value; */
      /* DEBUG("outside +1 %f", value); */
    /* } */
    if ( clock_value > 0.5){
      // rising voltage
      // set high so we dont process again this clock cycle
      clock = clock_value;
      // do work
      
      value = inputs[PATH118_INPUT].getVoltage();
      if (onest and value > 0.f) {
        /* DEBUG("+1 %f", value); */
        pitch = pitch + 1.f/12;
        notecount += 1;
        onest = false;
      }
      value = inputs[PATH120_INPUT].getVoltage();
      if (fivest and value > 0.f) {
        /* DEBUG("+5 %f", value); */
        pitch = pitch + 5.f/12;
        notecount += 5;
        fivest = false;
      }
      value = inputs[NEG1_INPUT].getVoltage();
      if (negonest and value > 0.f) {
        pitch = pitch - 1.f/12;
        notecount -= 1;
        negonest = false;
      }
      value = inputs[NEG5_INPUT].getVoltage();
      if (negfivest and value > 0.f) {
        pitch = pitch - 1.f/12;
        notecount -= 5;
        negfivest = false;
      }

      float foo = (int)notecount/7 + (major[(80 + notecount)%7] * 1.f/12);
      if (debug) {
        DEBUG("count %i oct %i mod %i st %i", notecount, (int)notecount/7, notecount %7, major[(80+ notecount)%7]);
        DEBUG("%f" ,foo);
        debug = 0;
      }

      if (pitch > top) {
        pitch = pitch - (top - bottom);
      }
      if (pitch < bottom) {
        pitch = pitch + (top - bottom);
      }
      outputs[PATH116_OUTPUT].setVoltage(pitch);
      
    } 
    if ( clock_value < 0.5 ){
      // falling voltage
      /* DEBUG("reseting"); */
      clock = 0;
      onest = true;
      fivest = true;
      negonest = true;
      negfivest = true;
      debug = true;
    }

	}
};


struct AdditiveWidget : ModuleWidget {
	AdditiveWidget(Additive* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/additive.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.598, 29.168)), module, Additive::PATH118_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.598, 39.267)), module, Additive::PATH120_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.598, 69.25)), module, Additive::CLKIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.598, 48.819)), module, Additive::NEG1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.598, 58.932)), module, Additive::NEG5_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.598, 18.065)), module, Additive::PATH116_OUTPUT));
	}
};


Model* modelAdditive = createModel<Additive, AdditiveWidget>("additive");
