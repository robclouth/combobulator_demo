import { Group, Image, Stack } from "@mantine/core";
import { useContext } from "react";
import { ParametersContext } from "../../juceIntegration/JuceIntegration";
import { Module } from "./Module";
import { ParameterDial } from "../ParameterDial";
import logo from "../../assets/images/logo.png";

export const Latents = () => {
  const parameters = useContext(ParametersContext)!;

  return (
    <Module title="Latents">
      <Group spacing={0}>
        <ParameterDial
          min={0}
          max={4}
          parameter={parameters.latentsScale}
          label="Scale"
          unit="%"
          labelValueScale={100}
        />
        <Image src={logo} maw={150} mx="auto" radius="md"></Image>
        <ParameterDial
          min={-2}
          max={2}
          parameter={parameters.latentsOffset}
          label="Offset"
          unit="%"
          showSign
          labelValueScale={100}
        />
      </Group>
    </Module>
  );
};
