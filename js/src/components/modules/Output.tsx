import { Group } from "@mantine/core";
import { useContext } from "react";
import { ParametersContext } from "../../juceIntegration/JuceIntegration";
import { Module } from "./Module";
import { ParameterDial } from "../ParameterDial";

export const Output = () => {
  const parameters = useContext(ParametersContext)!;

  return (
    <Module title="Output">
      <Group>
        <ParameterDial
          min={0.0}
          max={1.0}
          parameter={parameters.outputGain}
          label="Gain"
          unit="%"
          labelValueScale={100}
        />
      </Group>
    </Module>
  );
};
