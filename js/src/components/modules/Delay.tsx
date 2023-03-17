import { Group, Stack } from "@mantine/core";
import { useContext } from "react";
import { ParametersContext } from "../../juceIntegration/JuceIntegration";
import { Module } from "./Module";
import { ParameterDial } from "../ParameterDial";

export const Delay = () => {
  const parameters = useContext(ParametersContext)!;

  return (
    <Module title="Delay">
      <Stack spacing={0}>
        <ParameterDial
          min={0}
          max={2.0}
          parameter={parameters.delayFeedback}
          label="Feedback"
          unit="%"
          labelValueScale={100}
        />
        <ParameterDial
          min={0.01}
          max={10.0}
          parameter={parameters.delayTime}
          label="Time"
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
      </Stack>
    </Module>
  );
};
