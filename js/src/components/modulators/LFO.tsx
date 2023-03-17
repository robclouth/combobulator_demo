import { Group, SegmentedControl, SimpleGrid, Stack } from "@mantine/core";
import { observer } from "mobx-react";
import { FC, memo, useContext } from "react";
import { ParametersContext } from "../../juceIntegration/JuceIntegration";
import { ParameterDial } from "../ParameterDial";
import { Modulator } from "./Modulator";

import { createRef } from "react";
import { registerCallback } from "../../juceIntegration/juceCommunication";

const historyLength = 300;

export const Scope = memo(({ index }: { index: number }) => {
  const history: number[] = [];
  for (let i = 0; i < historyLength; i++) history.push(0);

  const canvasRef = createRef<HTMLCanvasElement>();

  registerCallback(`LFO${index + 1}::lfoData`, (data: number[]) => {
    history.push(...data);
    history.splice(0, data.length);

    if (!canvasRef.current) return;

    const canvas = canvasRef.current;
    const context = canvasRef.current.getContext("2d")!;

    canvas.width = canvas.offsetWidth;
    canvas.height = canvas.offsetHeight;

    context.clearRect(0, 0, canvas.width, canvas.height);
    const gradient = context.createLinearGradient(
      0,
      0,
      canvas.width,
      canvas.height
    );
    gradient.addColorStop(0, "blue");
    gradient.addColorStop(1, "white");
    context.strokeStyle = gradient;
    context.lineWidth = 2;

    context.beginPath();
    context.moveTo(
      0,
      (1 - (history[0] * 0.5 + 0.5)) * (canvas.height - context.lineWidth) +
        context.lineWidth / 2
    );
    for (let i = 1; i < history.length; i++) {
      const nextX = (i / history.length) * canvas.width;
      context.lineTo(
        nextX,
        (1 - (history[i] * 0.5 + 0.5)) * (canvas.height - context.lineWidth) +
          context.lineWidth / 2
      );
    }
    context.stroke();
  });

  return <canvas ref={canvasRef} style={{ width: "100%", height: 50 }} />;
});

interface Props {
  index: number;
}

export const LFO: FC<Props> = observer(({ index }) => {
  const parameters = useContext(ParametersContext)!;

  return (
    <Modulator
      id={`LFO${index + 1}`}
      title={`LFO ${index + 1}`}
      enabledParamId={`lfo${index + 1}Enabled`}
    >
      <Stack align="stretch">
        <SegmentedControl
          size="sm"
          radius="lg"
          value={(parameters as any)[`lfo${index + 1}Type`].value.toString()}
          onChange={(value) =>
            ((parameters as any)[`lfo${index + 1}Type`].value = parseInt(value))
          }
          data={[
            { label: "Sin", value: "0" },
            { label: "Tri", value: "1" },
            { label: "Saw", value: "2" },
            { label: "Rect", value: "3" },
            { label: "Noise", value: "4" },
          ]}
        />
        <SimpleGrid cols={3} spacing={0}>
          <ParameterDial
            radius={20}
            min={0.01}
            max={40}
            parameter={(parameters as any)[`lfo${index + 1}Rate`]}
            label="Rate"
            modulatable={false}
            valueFormatter={(value) => {
              if (value < 1)
                return (
                  (value * 1000).toLocaleString(undefined, {
                    maximumFractionDigits: 0,
                    minimumFractionDigits: 0,
                  }) + " ms"
                );
              else
                return (
                  value.toLocaleString(undefined, {
                    maximumFractionDigits: 2,
                    minimumFractionDigits: 2,
                  }) + " s"
                );
            }}
          />
          <ParameterDial
            radius={20}
            min={0}
            max={1}
            parameter={(parameters as any)[`lfo${index + 1}Phase`]}
            label="Phase"
            unit="%"
            labelValueScale={100}
            modulatable={false}
          />
          <ParameterDial
            radius={20}
            min={0}
            max={1}
            parameter={(parameters as any)[`lfo${index + 1}PulseWidth`]}
            label="PW"
            unit="%"
            labelValueScale={100}
            modulatable={false}
          />
          <ParameterDial
            radius={20}
            min={-1}
            max={1}
            parameter={(parameters as any)[`lfo${index + 1}Scale`]}
            label="Scale"
            unit="%"
            labelValueScale={100}
            modulatable={false}
          />
          <ParameterDial
            radius={20}
            min={-1}
            max={1}
            parameter={(parameters as any)[`lfo${index + 1}Offset`]}
            label="Offset"
            unit="%"
            labelValueScale={100}
            modulatable={false}
          />
        </SimpleGrid>
        <Scope index={index} />
      </Stack>
    </Modulator>
  );
});
