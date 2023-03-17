import { Group } from "@mantine/core";
import { useContext } from "react";
import { ParametersContext } from "../../juceIntegration/JuceIntegration";
import { Module } from "./Module";
import { ParameterDial } from "../ParameterDial";

export const PitchShift = () => {
  const parameters = useContext(ParametersContext)!;

  return (
    <Module title="Pitch shift">
      <Group>
        <ParameterDial
          min={-1200}
          max={1200}
          parameter={parameters.pitchOffset}
          label="Offset"
          decimalPlaces={0}
          unit=" cents"
          showSign
        />
      </Group>
    </Module>
  );
};
