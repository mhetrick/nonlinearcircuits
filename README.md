# Nonlinear Circuits
These are ports of the incredible Nonlinear Circuits Eurorack modules for [VCV Rack](https://vcvrack.com/). Many of these ports were originally made for Euro Reakt for Reaktor 6, and were created by analyzing real hardware builds.

Modules by [Nonlinear Circuits](https://www.nonlinearcircuits.com/) are typically smaller modules that focus on surprises through simple interaction. They are highly recommended as starter DIY Eurorack builds.

Recommended viewing: [Interview with Nonlinear Circuits by Music Thing Modular](https://www.youtube.com/watch?v=NxVBmO7cJFA)

## Credits
- Original Design: [Andrew Fitch](https://www.nonlinearcircuits.com/)
- Code: [Michael Hetrick](https://mhetrick.com/)
- Double Neuron, Let's Splosh!, and BOOLs Panels: [Papernoise](https://www.papernoise.net/) (Hannes Pasqualini)
- Squid Axon panel: Jason Wehmhoener
- 4Seq, GENiE, Neuron, Statues, Numberwang panels: jk
- Sloth Torpor DSP engine: [Don Cross](https://github.com/cosinekitty/sloth)

## Modules

### Double Neuron
This is based off of a special panel designed by [Papernoise and sold by Magpie Modular](https://magpie-modular.myshopify.com/collections/all-panels/products/nonlinear-circuits-dual-neuron?variant=31017400707). It is a panel meant to house two separate Neuron/Diff-Rect modules. Why two? Well, they're easy to build and they interact in a spectacular way.

[Video intro by Modular Addict](https://www.youtube.com/watch?v=2T5mkvdE-fc)

The top-half is a "Neuron" circuit. This is a three-input mixer that runs into a half-wave rectifier and comparator. The comparator's bipolar output is summed with the rectified signal. SENSE adds a positive DC signal to the mix, while RESPONSE affects the magnitude of the comparator's output. The values of the knobs here are accurate to the original hardware, and are not bipolar like the Euro Reakt port.

The Diff-Rect (Difference Rectifier) is another creative mixer. The two "+" inputs are summed together, and the two "-" inputs are summed together. The "-" mix is then subtracted from the "+" mix. If the result is positive, it goes out the "+" output, otherwise it goes out the "-" output.

If this seems complicated, just experiment! These modules were designed for fun, not for rigorous analysis.

![Double Neuron](./images/neuron.png)

### Squid Axon
This is a three channel mixer connected to a shift register of sorts. Unlike most shift registers, OUT 1 holds its value until OUT 4 has received its value. There are two feedback controls. OUT 4 is normalled to IN 3, so if nothing is connected to IN 3, it is a feedback input. The other feedback control is nonlinear and always available. This feedback path runs through a diode simulator, so it behaves much differently than the other feedback control.

To use: plug any signal into the CLOCK input. Whenever this signals goes above 1V, a clock will fire. Plug any other signals into IN 1 and IN 2. Keep IN 3 unplugged if you want to use the regular FEEDBACK control.

![Squid Axon](./images/squid.png)

### BOOLs
This is a four channel logic jumbler. The most common way to use this would be to plug 4 gates into the 4 inputs and derive logical combinations on the outputs. However, the inputs accept any signal (audio, LFOs, envelopes, you name it) and will decide that they are active if their value is above 1 volt.

On top of the logic jumbling, the four output channels are used to create a 4-bit stepped output at the STEP jack. This output also appears at the SLEW jack, but with a variable amount of smoothing set by the SLEW control.

Finally, if a cable is plugged into the SAMPLE input, the four output gates will only change upon the reception of a trigger here. This is useful for enforcing a more rigid tempo upon the outputs.

Right-click on the panel to select the active logic mode: OR, AND, XOR, NOR, NAND, XNOR

![BOOLs](./images/bools.png)

### Let's Splosh
This is a 4-input, 16-output master of signal jumbling. Essentially, four inputs are used in 8 different Diff-Rects (see the description under the Neuron above). The left half of the module contains the positive outputs, while the right half is negative. Try throwing in four LFOs of different frequencies as a starter patch. It's a lot of fun!

![Let's Splosh](./images/splosh.png)

### Splish
This is a 3-input, 4HP variation of Let's Splosh. It has 8 outputs instead of 16.

### 4Seq
This is a simple sequencer with a deceiving amount of depth. There are two sequences: X and Y. Y is always the opposite step of X and counting in the opposite direction (so if X is step 0, Y is step 3; if X is step 1, Y is step 2). There is a gate input that reverses the count direction while the gate is above 1V. Finally, the bottom section is a clock divider that divides the incoming clock. Try plugging the clock divider's outputs into the direction input!

![4Seq](./images/4seq.png)

### GENiE
If you thought the Double Neuron was crazy, this is essentially a triple Neuron pre-patched with a feedback network. The outputs of the three neurons are normalled to each other's inputs (as indicated on the panel). Those inputs have an additional IN control on top to control the gain. Because of the extreme amount of feedback, changing VCV's sampling rate can wildly alter the sound of this.

![GENiE](./images/genie.png)

### Statues
This is a 1-in 8-out switch with two major twists. First, the output is determined by the state of three gate inputs. These gate inputs essentially create a 3-bit integer (1 through 8) to choose the output. Second, when an output becomes inactive, it holds its previous value. This can be a lot more useful than a switch where a modulation output is suddenly dropped to zero!

![Statues](./images/statues.png)

### Numberwang
Four gate inputs make 16 gate outputs. That's Numberwang!
Each of the four gate outputs represents a bit in a 4-bit number (1-16). Depending on the values of the incoming gates, the corresponding output gate on the matrix will be active. Try plugging in four LFOs at different frequencies and watch the patterns.
![Numberwang](./images/numberwang.png)

### Segue
This module is very similar to HetrickCV's Flip Pan. It pans two input signals by flipping their channels (i.e. the left input is panned right, the right input is panned left). This sounds simple, but it is very versatile.

- Plug one signal in and monitor both outputs to use this as a traditional panner.
- Plug two signals in and monitor only one output to use this as a crossfader.
- Plug one signal in and monitor the opposite output to use this as a VCA.

### 8-Bit Cipher
This is a massive chunk of unpredictability, somewhat like a deluxe Turing Machine. [Original description from NLC.](https://www.nonlinearcircuits.com/modules/p/8bit-cipher)

### Divine CMOS
A complicated subharmonic generator, or clock divider, or weird sequencer. Essentially, send a signal into either input (audio or gates). The main OUT is a mix of four clock dividers, the gain of each is controlled by the knobs. With a predictable clock signal into only one of the inputs, you can treat this output as a stepped sequencer. The SLEW output is the same signal as OUT, except with a variable amount of slew controlled by the SLEW knob. The bottoms four outputs are dedicated clock dividers that are not affected by the gain controls.

Here's where things get hairy: with two signals plugged in, the clock divider outputs are now the logical XOR of the divisions of the two incoming signals. Essentially, this is a 1-bit ring modulator. Just experiment and have fun. If you're using this for audio, be sure to run the output through a DC Blocker (or a high pass filter set to around 20 Hz) to remove any offset, as the outputs are all unipolar.

### Divide & Conquer
This is a clock divider with more outputs than you could ask for. It includes powers of 2 from 2 to 256, along with 3, 5, and 7. On top of that, there are 1.5 (3/2), 2.5 (5/2), and 3.5 (7/2). On these dividers, the half divisions are calculated when the incoming clock goes low.

The main clock input (on the bottom section) is normalled to the top three clock inputs. If nothing is plugged into the top three input jacks, they will receive the input connected to the main input.

To achieve more divisions, you can plug this module into itself. For instance, to divide by 14, plug the /2 output into the /7 clock input and monitor the /7 output.


### Router
This is a four-channel signal toggler. The knobs set values from -10 to +10 volts. If the signals present at the CV inputs are lower than the set voltage, then the corresponding channel will be active.

### Sloth : Torpor, Apathy, and Inertia

Sloth Torpor, Sloth Apathy, and Sloth Inertia are software emulations of [Sloth Chaos](https://www.nonlinearcircuits.com/modules/p/4hp-sloth-chaos). They are chaotic oscillators that emit two slowly changing signals that can be used for modulation or control voltages. Torpor takes about 15-30 seconds to travel around 2 strange attractors. Apathy takes 60-90 seconds and Inertia takes 30-40 minutes.

The behavior of these circuits is unpredictable over time, but more orderly than random. They are often used for low frequency control voltages in cases where an LFO would be too predictable, but a random signal would be too erratic.

The knobs do not alter the frequencies, but can affect how much time the signals spend traveling around one strange attractor rather than the other. Do not expect immediate or obvious results from adjusting the knobs. These are subtle controls for your deep space journey.

The light indicates which of the two strange attractors is being orbited. It turns green when orbiting the upper lobe (the "big" voltage is positive), red around the lower lobe (the "big" voltage is negative). It gets darker near 0V.

Here is an outline of the [circuit analysis and implementation notes](https://github.com/cosinekitty/sloth/blob/main/README.md) of Sloth Torpor, for anyone who is curious about the technical details. Apathy and Inertia are very similar, just with different numeric parameters to make them operate at different speeds.

![Torpor](./images/SlothTorpor.png) ![Apathy](./images/SlothApathy.png) ![Inertia](./images/SlothInertia.png)

### Triple Sloth

Triple Sloth is an emulation of the [Triple Sloth hardware module](https://www.nonlinearcircuits.com/modules/p/triple-sloth). Triple Sloth is a combination of the three individual Sloth modules: Torpor, Apathy, and Inertia. It also adds CV inputs to Torpor and Apathy to subtly affect their chaotic trajectories. Inertia does not have a knob or a CV input. It does what it wants!

The "Z" output is the same as the "Y" output in the single Sloth modules, only inverted.

In addition, Triple Sloth provides "+" and "&minus;" outputs that combine the Z outputs from Apathy ($V_a$), Inertia ($V_i$), and Torpor ($V_t$). The formula for the combined voltage is

$$
V_c = 0.7 \left[ V_t - \left( V_a + V_i \right) \right]
$$

When the combined voltage $V_c$ is positive, the "+" output sends that voltage, otherwise it sends 0V. When $V_c$ is negative, the "&minus;" output sends that voltage, otherwise it sends 0V.

![Triple Sloth](./images/TripleSloth.png)

### Future Additions
- 1050 MixSeq
- 32:1

